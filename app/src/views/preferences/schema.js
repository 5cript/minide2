const PreferencesSchema = (dict) =>
{
    return {
        fields: [
            {
                key: 'host',
                label: dict.translate('$Host', 'preferences'),
                type: 'input',
                category: 'backend'
            },
            {
                key: 'port',
                label: dict.translate('$Port', 'preferences'),
                type: 'numberInput',
                category: 'backend'
            },
            {
                key: 'spacer1',
                type: 'spacer',
                category: 'backend'
            },
            {
                key: 'autoConnect',
                label: dict.translate('$AutoConnectToBackend', 'preferences'),
                type: 'boolbox',
                category: 'backend'
            },
            {
                key: 'autoLoadWorkspace',
                label: dict.translate('$AutoReloadLastWorkspace', 'preferences'),
                type: 'boolbox',
                category: 'backend'
            }
        ],
        categories: [{
            id: 'backend',
            caption: dict.translate('$BackendSettings', 'preferences'),
            fields: []
        }]
    }
}

export default PreferencesSchema;