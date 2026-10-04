-- Workshop capture — database schema
-- Run this once in the Supabase SQL editor (Dashboard → SQL Editor → New query).
-- Handover §4 (data model) and §5 (row level security).

-- ── Tables ──────────────────────────────────────────────────────────────────

create table if not exists participants (
  id uuid primary key,
  created_at timestamptz default now()
);

-- Long format, APPEND-ONLY: every save is its own INSERT. There is deliberately
-- NO unique(participant_id, day, stage, question_id) constraint — the app never
-- updates a row, so a save can't silently fail as a zero-row UPDATE (which is how
-- whole paragraphs were lost). Answers accumulate as versions; the latest row per
-- question is the final answer (see ANALYSIS.md). No foreign key from
-- responses.participant_id — a race on first load must never cost an answer.
create table if not exists responses (
  id uuid primary key default gen_random_uuid(),
  participant_id uuid not null,
  day smallint not null,
  stage text not null,          -- 'pre' | 'task1' | 'task2' | ... | 'exit'
  question_id text not null,
  value text,                   -- ratings live here as text; cast at analysis time
  updated_at timestamptz default now()
);

-- Migration for a database created with the earlier one-row-per-question design:
-- drop whatever unique constraint exists on responses so appends stop colliding.
do $$
declare c text;
begin
  select conname into c
  from pg_constraint
  where conrelid = 'responses'::regclass and contype = 'u';
  if c is not null then
    execute 'alter table responses drop constraint ' || quote_ident(c);
  end if;
end $$;

create index if not exists responses_participant_idx on responses (participant_id);
create index if not exists responses_day_stage_idx on responses (day, stage);

-- ── Row level security ──────────────────────────────────────────────────────
-- With the anon key you can WRITE and you cannot READ. The dataset is never
-- exposed by the public app. Deliberately NO select policy on either table.

alter table participants enable row level security;
alter table responses    enable row level security;

-- Drop-and-recreate so this script is safe to re-run.
drop policy if exists "anon can start"  on participants;
drop policy if exists "anon can write"  on responses;
drop policy if exists "anon can revise" on responses;

create policy "anon can start"  on participants for insert to anon with check (true);
create policy "anon can write"  on responses    for insert to anon with check (true);
create policy "anon can revise" on responses    for update to anon using (true) with check (true);

-- Note: `update ... using (true)` means anyone holding the anon key could
-- overwrite rows if they knew the composite key. For a room of beginners and
-- non-sensitive data this is accepted. Do not harden this now (§5).
