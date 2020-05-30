import React from 'react';
import MainWindow from './views/main/main';
import {Provider} from 'react-redux';

import store from './store';

// styles

function App() {
	return (
		<Provider store={store}>
			<MainWindow store={store}/>
		</Provider>
	);
}

export default App;
