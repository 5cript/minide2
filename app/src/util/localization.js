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
                    "$Environment": "Environment",
                    "$Preferences": "Preferences",
                    "$Backend": "Backend",
                    "$Connect": "Connect",
                    "$Test": "TestAction_DEL_ME",
                    "$ReloadToolbar": "Reload toolbar"
                },
                "explorer": {
                    "$OpenFiles": "Open Files",
                    "$FileBrowser": "File Browser"
                },
                "environments": {
                    "$Add": "Add",
                    "$Delete": "Delete",
                    "$PathLabel": "Path Variable",
                    "$VariablesLabel": "Other Environment Variables",
                    "$Key": "Key",
                    "$Value": "Value",
                    "$ConfirmProfileRemove": "Do you really want to delete this environment profile?",
                    "$ProfileName": "Profile Name",
                    "$ProfileAlreadyExists": "Profile with that name already exists",
                    "$CommunicatingWithServer": "Communicating with server",
                    "$FetchFailed": "Server communication failed",
                    "$SaveChanges": "Save unsaved changes?",
                    "$CloseAnyway": "Saveing failed, close regardless?"
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
                    "$Cancel": "Cancel",
                    "$UnknownSchemeType": "Unknown Type for Form",
                    "$UnfitRequirements": "Requirements not met",
                    "$MayNotBeEmpty": "May not be empty",
                    "$Save": "Save",
                    "$NoActiveProject": "No active project",
					"$FileOutsideWorkspace": "File is outside of the workspace. Do you really want to save?"
                },
                "file_tree": {
                    "$SetAsActiveProject": "Set as Active Project",
                    "$OpenFile": "Open",
                    "$DeleteFile": "Delete",
                    "$OpenToTheSide": "Open in Split View",
                    "$RenameFile": "Rename File",
                    "$ReallyDeleteFile": "Really delete file? This cannot be undone"
                },
                "toolbar": {
                    "$Save": "Speichern",
                    "$ProjectSettings": "Project Settings"
                },
                "lua": {
                    "$ProcessEnded": "ended with status",
                    "$ProcessStartFail": "could not be started, error code: ",
                    "$ProcessNotFound": "Process not found"
                },
                "preferences": {                    
                    "$AutoReloadLastWorkspace": "Reload last opened workspace",
                    "$AutoConnectToBackend": "Automatically connect to local backend",
                    "$AutoLoadLastProject": "Automatically activate last active project",
                    "$BackendSettings": "Backend",
                    "$Host": "Host",
                    "$Port": "Port"
                },
                "project": {
                    "$LooksLikeSourceSplitButNotConfigured": "Looks like your project does a split by include and source by common convention, but your project is not configured accordingly. Do you want to change that?",
                    "$FileNotFoundShallCreate": "File not found, do you want to create it?",
                    "$CouldNotCreateFile": "Could not create file"
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
                    "$Environment": "Ausführumgebung",
                    "$Preferences": "Lokale Einstellungen",
                    "$Backend": "Backend",
                    "$Connect": "Verbinden",
                    "$Test": "Testaktion_DEL_ME",
                    "$ReloadToolbar": "Toolbar neu laden"
                },
                "explorer": {
                    "$OpenFiles": "Offene Dateien",
                    "$FileBrowser": "Datei Browser"
                },
                "environments": {
                    "$Add": "Hinzufügen",
                    "$Delete": "Löschen",
                    "$PathLabel": "Pfad Variable",
                    "$VariablesLabel": "Sonstige Umgebungsvariablen",
                    "$Key": "Schlüssel",
                    "$Value": "Wert",
                    "$ConfirmProfileRemove": "Wollen sie dieses Profil wirklich löschen?",
                    "$ProfileName": "Profilname",
                    "$ProfileAlreadyExists": "Profil mit diesem Namen existiert bereits",
                    "$CommunicatingWithServer": "Kommuniziere mit Server",
                    "$FetchFailed": "Serverkommunikation fehlgeschlagen",
                    "$SaveChanges": "Ungespeicherte Änderungen speichern?",
                    "$CloseAnyway": "Speichern fehlgeschlage, trotzdem schließen?"
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
                    "$Cancel": "Abbrechen",
                    "$UnknownSchemeType": "Unbekannter Typ für Formular",
                    "$UnfitRequirements": "Bedingungen für Eingabe nicht eingehalten",
                    "$MayNotBeEmpty": "Darf nicht leer sein",
                    "$Save": "Speichern",
                    "$NoActiveProject": "Kein aktives Projekt",
					"$FileOutsideWorkspace": "Die Datei ist außerhalb des Arbeitsrausm. Wollen sie sie wirklich speichern?"
                },
                "file_tree": {
                    "$SetAsActiveProject": "Als aktives Projekt wählen",
                    "$OpenFile": "Öffnen",
                    "$DeleteFile": "Löschen",
                    "$OpenToTheSide": "Parallel Öffnen",
                    "$RenameFile": "Datei Umbennen",
                    "$ReallyDeleteFile": "Datei wirklich unwiederbringlich löschen?"
                },
                "toolbar": {
                    "$Save": "Speichern",
                    "$ProjectSettings": "Projekteinstellungen"
                },
                "lua": {
                    "$ProcessEnded": "wurde beendet mit Status",
                    "$ProcessStartFail": "konnte nicht gestartet werden, Fehler: ",
                    "$ProcessNotFound": "Programm nicht gefunden"
                },
                "preferences": {
                    "$AutoReloadLastWorkspace": "Zuletzt geöffneten Arbeitsraum automatisch öffnen",
                    "$AutoConnectToBackend": "Automatisch mit Backend verbinden",
                    "$AutoLoadLastProject": "Letztes Aktives Projekt wieder setzen",
                    "$BackendSettings": "Backend",
                    "$Host": "Host",
                    "$Port": "Port"
                },
                "project": {
                    "$LooksLikeSourceSplitButNotConfigured": "Es sieht so aus als wäre dieses Projekt in 'include' und 'source' getrennt. Aber das Projekt ist nicht dahingehend konfiguriert. Wollen sie die Trennung aktivieren?",
                    "$FileNotFoundShallCreate": "Datei nicht gefunden, soll sie erstellt werden?",
                    "$CouldNotCreateFile": "Konnte datei nicht erstellen"
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
        if (key === undefined)
            return;

        if (key.length === 0)
            return;
        
        if (key[0] !== '$')
            return key;

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