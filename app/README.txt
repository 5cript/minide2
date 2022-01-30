Fixed to make this run:
	electron-redux@alpha^9: lib/preload.js add `if (typeof window !== 'undefined')` before line 94