class IpcCore
{
    protocol = 'http';
    port = 43255;

    _request = (path, verb) => {
        return fetch(this.protocol + '://localhost:' + this.port + path, {
            mode: 'cors',
            method: verb
        }).then(r => {
            return r.json()
        })
    }

    get = (path) => {
        return this._request(path, 'get')
    }
}

let ipc = new IpcCore();
export default ipc;
