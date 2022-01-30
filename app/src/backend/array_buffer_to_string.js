export function arrayBufferToString(arrayBuffer)
{
    let str = '';
    const view = new Uint8Array(arrayBuffer);
    for (let i = 0; i != view.length; ++i)
        str += String.fromCharCode(view[i]);
    return str;
}