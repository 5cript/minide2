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
                    "$OpenWorkspace": "Open Workspace",
                    "$Settings": "Settings",
                    "$Environment": "Environment"
                },
                "explorer": {
                    "$OpenFiles": "Open Files",
                    "$FileBrowser": "File Browser"
                },
                "json_options": {
                    "$JsonRepresentation": "JSON Representation",
                    "$JsonBrokenDiscard": "JSON is incorrect, discard invalid changes?"
                },
                "main_window": {
                    "$ConnectingToBackend": "Connecting to Backend",
                    "$ConnectionLost": "Connection Lost",
                    "$ConnectionFailed": "Connection Interupted / Failed"
                },
                "dialog": {
                    "$CloseUnsavedWarning": "Do you really want to close this file without saving?",
                    "$ReloadFileFromServer": "Discard changes and reload file?",
                    "$CloseWithUnsavedChanges": "There are unsaved changes, close anyway?",
                    "$Yes": "Yes",
                    "$No": "No",
                    "$Ok": "Ok",
                    "$Cancel": "Cancel"
                },
                "file_tree": {
                    "$SetAsActiveProject": "Set as Active Project",
                    "$OpenFile": "Open",
                    "$DeleteFile": "Delete",
                    "$OpenToTheSide": "Open in Split View",
                    "$RenameFile": "Rename File"
                }
            },
            "de_DE": {
                "meta": {
                    "$LANGUAGE_SELF": "de_DE",
                },
                "menu": {
                    "$File": "Datei",
                    "$OpenWorkspace": "Arbeitsraum Öffnen",
                    "$Settings": "Einstellungen",
                    "$Environment": "Ausführumgebung"
                },
                "explorer": {
                    "$OpenFiles": "Offene Dateien",
                    "$FileBrowser": "Datei Browser"
                },
                "json_options": {
                    "$JsonRepresentation": "JSON Repräsentation",
                    "$JsonBrokenDiscard": "JSON ist inkorrekt, Änderungen verwerfen?"
                },
                "main_window": {
                    "$ConnectingToBackend": "Verbinde mit Server",
                    "$ConnectionLost": "Verbindung Verloren",
                    "$ConnectionFailed": "Verbindung Unterbrochen / Fehlgeschlagen"
                },
                "dialog": {
                    "$CloseUnsavedWarning": "Datei hat ungespeicherte Änderungen, wirklich ohne speichern schließen?",
                    "$ReloadFileFromServer": "Änderungen verwerfen und Datei neu laden?",
                    "$CloseWithUnsavedChanges": "Es gibt noch ungespeicherte Änderungen, trotzdem schließen?",
                    "$Yes": "Ja",
                    "$No": "Nein",
                    "$Ok": "Ok",
                    "$Cancel": "Abbrechen"
                },
                "file_tree": {
                    "$SetAsActiveProject": "Als aktives Projekt wählen",
                    "$OpenFile": "Öffnen",
                    "$DeleteFile": "Löschen",
                    "$OpenToTheSide": "Parallel Öffnen",
                    "$RenameFile": "Datei Umbennen"
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