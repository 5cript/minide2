import React from 'react';
import MainWindow from './views/main/main_window';
import Environments from './views/environments/environments';
import Preferences from './views/preferences/preferences';
import Keybinds from './views/keybinds/keybinds';
import {BrowserRouter as Router, Route} from 'react-router-dom';
import {Provider} from 'react-redux';
import store from './store_renderer';

// styles

class ViewRouter extends React.Component
{
	static Views(store)
	{
		return {
			main: <MainWindow store={store}/>,
			environments: <Environments store={store}/>,
			preferences: <Preferences store={store}/>,
			keybinds: <Keybinds store={store}/>
		}
	}

	static View(props)
	{
		let name = props.location.search.substr(1);
		let view = ViewRouter.Views(store)[name];
		if(view == null) 
			throw new Error('View "' + name + '" is undefined');
		return view;
	}

	render() {
		return (
			<Provider store={store}>
				<Router>
					<div>
						<Route path='/' component={ViewRouter.View}/>
					</div>
				</Router>
			</Provider>
		);
	}
}

export default ViewRouter;