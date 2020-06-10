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
                },
                "main_window": {
                    "$ConnectingToBackend": "Connecting to Backend",
                    "$ConnectionLost": "Connection Lost",
                    "$ConnectionFailed": "Connection Failed"
                },
                "dialog": {
                    "$CloseUnsavedWarning": "Do you really want to close this file without saving?",
                    "$ReloadFileFromServer": "Discard changes and reload file?",
                    "$CloseWithUnsavedChanges": "There are unsaved changes, close anyway?",
                    "$Yes": "Yes",
                    "$No": "No",
                    "$Ok": "Ok",
                    "$Cancel": "Cancel"
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
                },
                "main_window": {
                    "$ConnectingToBackend": "Verbinde mit Backend",
                    "$ConnectionLost": "Verbindung Verloren",
                    "$ConnectionFailed": "Verbinden Fehlgeschlagen"
                },
                "dialog": {
                    "$CloseUnsavedWarning": "Datei hat ungespeicherte Änderungen, wirklich ohne speichern schließen?",
                    "$ReloadFileFromServer": "Änderungen verwerfen und Datei neu laden?",
                    "$CloseWithUnsavedChanges": "Es gibt noch ungespeicherte Änderungen, trotzdem schließen?",
                    "$Yes": "Ja",
                    "$No": "Nein",
                    "$Ok": "Ok",
                    "$Cancel": "Abbrechen"
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

export default Dictionary;