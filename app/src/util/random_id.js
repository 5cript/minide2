export default function generateId(length)
{
    let id = '';
    const lookup = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789';
    for (let i = 0; i != length; ++i) 
        id += lookup[Math.floor(Math.random() * lookup.length)];
    return id;
}