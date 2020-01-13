import React from 'react';
import MainWindow from './views/main/main';

// styles

// requires
let electron = window.require('electron');

electron.ipcRenderer.on('hello', (event, arg) => {
	console.log(event);
})

function App() {
	return (
		<MainWindow />
	);
}

export default App;
