import React from 'react';

import {connect} from 'react-redux';

import classnames from 'classnames';

// Actions
import { setActiveFile, removeOpenFile } from '../../../actions/open_file_actions';

// Style
import './styles/open_files.css';

class OpenFilesList extends React.Component
{
    render = () => {
        return (
            <div id='OpenFilesList'>
                {this.props.openFiles.map((f, i) => {return(
                    <div 
                        className={classnames({
                            'openFileItem': true,
                            'alternativeFileItem': i % 2 === 0,
                            'activeFileItem': this.props.activeFile === i
                        })}
                        key={f.path}
                        onClick={() => {
                            console.log(this.props);
                            this.props.dispatch(setActiveFile(i));
                        }}
                    >
                        <button 
                            className={classnames({
                                'openFileXButton': true,
                                'inactiveXButton': this.props.activeFile !== i
                            })}
                            id='x' 
                            onClick={(e) => {this.props.dispatch(removeOpenFile(f.path)); e.stopPropagation()}}></button>
                        <div className='shortFileItemName'>
                            {f.path}
                        </div>
                    </div>
                )})}
            </div>
        )
    }
}


export default connect(
    state => {
        return state.openFiles;
    }
)(OpenFilesList);