import nodePath from 'path';

let pathModifier =
{
    shorten: (path) => {
        return nodePath.basename(path);
    },

    concat: (base, relative) => {
        return nodePath.join(base, relative);
    }
}

export {pathModifier};