const PreferencesSchema = (dict) =>
{
    return {
        fields: [
            {
                key: 'autoConnect',
                label: dict.translate('$AutoConnectToLocalBackend', 'preferences'),
                type: 'boolbox',
                category: 'localBackend'
            },
            {
                key: 'autoLoadWorkspace',
                label: dict.translate('$AutoReloadLastLocalWorkspace', 'preferences'),
                type: 'boolbox',
                category: 'localBackend'
            },
            {
                key: 'xd',
                label: dict.translate('$AutoReloadLastLocalWorkspace', 'preferences'),
                type: 'boolbox',
                category: 'subcat'
            }
        ],
        categories: {
            id: 'localBackend',
            caption: dict.translate('$LocalBackendSettings', 'preferences'),
            fields: [],
            categories: [{
                id: 'subcat'
            }]
        }
    }
}

export default PreferencesSchema;