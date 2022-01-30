export function unpackConstant(value)
{
    if (value[0] === 'i')
        return Number.parseInt(value.substring(1, value.length));
    return value;
}

export function dedash(value)
{
    let result = "";
    let capitalize = false;
    for (let k in value)
    {
        if (value[k] === '-')
        {
            capitalize = true;
        }
        else
        {
            if (capitalize)
            {
                result += value[k].toUpperCase();
                capitalize = false;
            }
            else
            {
                result += value[k];
            }
        }
    }
    return result;
}