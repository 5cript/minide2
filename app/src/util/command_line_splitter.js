export function splitCommandLine(command)
{
    let res = [];

    let inQuote = false;
    let accumulator = '';
    let slashOnce = false;
    for (let i = 0; i != command.length; ++i) 
    {
        switch (command[i])
        {
            case("\\"): 
            {
                if (slashOnce)
                {
                    accumulator += "\\";
                    slashOnce = false;
                }
                else
                    slashOnce = true;
                break;
            }
            case('"'):
            {
                if (slashOnce)
                {
                    accumulator += '"'
                    slashOnce = false;
                }
                else
                    inQuote = !inQuote;
                break;
            }
            case(" "):
            case("\t"):
            case("\n"):
            {
                if (inQuote)
                {
                    accumulator += command[i];
                    break;
                }
                if (slashOnce)
                {
                    accumulator += command[i];
                    slashOnce = false;
                }
                else 
                {
                    // dont push empty parts:
                    if (!accumulator.match(/^\s*$/))
                        res.push(accumulator);
                    accumulator = "";
                }
                break;
            }
            default: 
            {
                if (slashOnce)
                    slashOnce = false;
                accumulator += command[i];
                break;
            }
        }
    }
    if (slashOnce)
        accumulator += "\\";
    if (accumulator.length > 0)
        res.push(accumulator);

    if (res.length === 1)
        return {
            command: res[0],
            arguments: []
        }
    if (res.length > 1)
        return {
            command: res[0], 
            arguments: res.slice(1, res.length)
        }
    return {
        command: "",
        arguments: []
    };
}