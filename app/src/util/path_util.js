import nodePath from 'path';

let pathModifier =
{
    shorten: (path) => {
        return nodePath.basename(path);
    }
}

export {pathModifier};