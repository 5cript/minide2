import React from 'react';

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

// requires
let dict = require('../../../util/localization.js');

class Explorer extends React.Component
{
    render = () => {
        return (
            <div>
                <div id={'ExplorerHeader'}>
                    Explorer
                </div> 
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
                            <OpenFilesList openFiles={this.props.openFiles}></OpenFilesList>
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

export default Explorer;