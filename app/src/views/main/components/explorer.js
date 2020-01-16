import React from 'react';
import { connect } from 'react-redux';

import {
    Accordion,
    AccordionItem,
    AccordionItemHeading,
    AccordionItemButton,
    AccordionItemPanel,
} from 'react-accessible-accordion';

import FileTree from './file_tree';
import OpenFilesList from './open_files';

// Styling
import './styles/explorer.css';

// Actions
import { addOpenFile } from '../../../actions/open_file_actions';

// requires
let dict = require('../../../util/localization.js');

/* 
<button onClick={
    () => {
        let fileName = [...Array(10)].map(i=>(~~(Math.random()*36)).toString(36)).join('');
        this.props.dispatch(addOpenFile(fileName));
    }
}>x</button>
*/

class Explorer extends React.Component {
    render = () => {
        return (
            <div>
                <div id={'ExplorerHeader'}>
                    <div>Explorer</div>
                    <div className='explorerOpenDir'>{(() => {
                        let root = this.props.workspace.root;
                        if (root !== undefined || root !== '' || root !== null)
                            return root;
                    })()}</div>
                </div>
                <button onClick={
                    () => {
                        let fileName = [...Array(10)].map(i=>(~~(Math.random()*36)).toString(36)).join('');
                        this.props.dispatch(addOpenFile(fileName));
                    }
                }>x</button>
                <Accordion
                    allowMultipleExpanded={true}
                    allowZeroExpanded={true}
                    preExpanded={['open_files', 'file_browser']}
                >
                    <AccordionItem uuid={'open_files'}>
                        <AccordionItemHeading>
                            <AccordionItemButton>
                                {dict.translate('$OpenFiles', 'explorer')}
                            </AccordionItemButton>
                        </AccordionItemHeading>
                        <AccordionItemPanel>
                            <OpenFilesList></OpenFilesList>
                        </AccordionItemPanel>
                    </AccordionItem>
                    <AccordionItem uuid={'file_browser'}>
                        <AccordionItemHeading>
                            <AccordionItemButton>
                                {dict.translate('$FileBrowser', 'explorer')}
                            </AccordionItemButton>
                        </AccordionItemHeading>
                        <AccordionItemPanel>
                            <FileTree />
                        </AccordionItemPanel>
                    </AccordionItem>
                </Accordion>
            </div>
        )
    }
}

export default connect(state => {
    return {workspace: state.workspace};
})(Explorer);