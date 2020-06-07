let _ = require('lodash')

class Dictionary
{
    constructor()
    {
        this.load('');
        this.lang = "de_DE";

        this.dicts = {
            "en_US": {
                "meta": {
                    "$LANGUAGE_SELF": "en_US"
                },
                "menu": {
                    "$File": "File",
                    "$OpenWorkspace": "Open Workspace"
                },
                "explorer": {
                    "$OpenFiles": "Open Files",
                    "$FileBrowser": "File Browser"
                }
            },
            "de_DE": {
                "meta": {
                    "$LANGUAGE_SELF": "de_DE",
                },
                "menu": {
                    "$File": "Datei",
                    "$OpenWorkspace": "Superprojekt Öffnen"
                },
                "explorer": {
                    "$OpenFiles": "Offene Dateien",
                    "$FileBrowser": "Datei Browser"
                }
            }    
        }
    }

    setLang(lang) 
    {
        this.lang = lang;
    }

    translate(key, dict) 
    {
        if (dict === undefined) {
            dict = 'merged'            
        }

        dict = this.dicts[this.lang][dict];
        if (dict === undefined)
            return 'unknown_dict_' + dict + '_key_' + key;
        var val = dict[key];
        if (val === undefined)
            return key;
        return val;
    }

    load(path) 
    {
        //console.log("not implemented");
        for (let lang in this.dicts) {
            let merged = {}
            for (let group in this.dicts[lang]) {
                merged = _.merge(merged, this.dicts[lang][group]);
            }
            this.dicts[lang]["merged"] = merged;
        }
    }
}

let dict = new Dictionary();

module.exports = dict;