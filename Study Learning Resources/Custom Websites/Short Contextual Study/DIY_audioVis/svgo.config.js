// Keep the viewBox — the ShieldExplainer overlay aligns to it.
export default {
	multipass: true,
	plugins: [
		{
			name: 'preset-default',
			params: {
				overrides: {
					removeViewBox: false
				}
			}
		}
	]
};
