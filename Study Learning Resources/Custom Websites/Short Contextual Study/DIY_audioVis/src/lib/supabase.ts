import { createClient, type SupabaseClient } from '@supabase/supabase-js';
import { PUBLIC_SUPABASE_URL, PUBLIC_SUPABASE_ANON_KEY } from '$env/static/public';

// Anon/public key only. This app can write and cannot read — the RLS policies
// (see supabase/schema.sql §5) deliberately grant no SELECT. Do not add a
// service_role key here, and never chain .select() onto a write.
//
// Created lazily on first use (always in the browser). Building the client
// eagerly would run supabase-js's realtime setup during server-side
// prerendering, which needs a WebSocket Node 20 doesn't provide.
let client: SupabaseClient | null = null;

export function getSupabase(): SupabaseClient {
	client ??= createClient(PUBLIC_SUPABASE_URL, PUBLIC_SUPABASE_ANON_KEY, {
		auth: {
			persistSession: false,
			autoRefreshToken: false
		}
	});
	return client;
}
