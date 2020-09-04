import {activeFileWasSynchronized, fileWasSynchronized} from '../../actions/open_file_actions';

export default class CommonActions
{
    constructor(store, mainWindow, backend, debugController)
    {
        this.mainWindow = mainWindow
        this.backend = backend;
        this.store = store;
        this.debugController = debugController;

        console.log(store);
    }

    getState = () => 
    {
        const state = this.store.getState();

        return {
            openFiles: state.openFiles.openFiles,
            activeFile: state.openFiles.activeFile,
            shortcuts: state.shortcuts
        }
    }

    saveFile = () =>
    {
        if (this.getState().activeFile >= 0) 
        {
            let file = this.getState().openFiles[this.getState().activeFile];
            if (file.isAbsolutePath)
            {
                this.mainWindow.showYesNoBox(this.dict.translate("$FileOutsideWorkspace", "dialog"), () => {
                    this.backend.workspace().saveFile(file.path, file.content, () => 
                    {
                        this.store.dispatch(activeFileWasSynchronized());
                    });
                })
            }
            else
            {
                this.backend.workspace().saveFile(file.path, file.content, () => 
                {
                    this.store.dispatch(activeFileWasSynchronized());
                });
            }
        }
        else
        {
            this.mainWindow.showOkBox('todo: implement save as for void model');
            return;
        }
    }

    saveAllFiles = () =>
    {
        this.getState().openFiles.map(file => 
        {
            if (!file.synchronized)
            {
                this.backend.workspace().saveFile(file.path, file.content, () => 
                {
                    this.store.dispatch(fileWasSynchronized(file.path));
                });
            }
            return file;
        })
    }

    startDebugger = () => 
    {
        this.debugController.startDebugger();
    }
}