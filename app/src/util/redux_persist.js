class ReduxPersistanceHelper
{
    constructor(store)
    {
        this.store = store;
        this.fs = window.require('fs');
    }

    saveSelection = (outputFile, selector) => 
    {
        const state = this.store.getState();
        const selection = selector(state);
        this.fs.writeFileSync(
            state.misc.configHome + "/" + outputFile, 
            JSON.stringify(selection, null, 4)
        );
    }

    /**
     * Necessary, because dispatches are replayed to synchronize all electron windows.
     * Cannot bypass this system or the windows go out of sync.
     */
    loadByDispatch = (outputFile, actionFactories, homeOverride) => 
    {
        let home = homeOverride;
        if (home === undefined)
        {
            const state = this.store.getState();
            home = state.misc.configHome;
        }
        const path = home + "/" + outputFile;
        let fileContent;
        if (this.fs.existsSync(path))
            fileContent = JSON.parse(this.fs.readFileSync(path, 'utf8'));
        else 
            return;
        
        actionFactories.forEach(factory => {
            this.store.dispatch(factory(fileContent));
        })
    }
}

module.exports = ReduxPersistanceHelper;