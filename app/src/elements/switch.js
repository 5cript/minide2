import Switch from '@material-ui/core/Switch';
import { withStyles } from '@material-ui/core/styles';

const ThemedSwitch = withStyles({
    switchBase: {
        color: 'var(--foreground-color)',
        '&$checked': {
            color: 'var(--theme-color-extreme)',
        },
        '&$checked + $track': {
            backgroundColor: 'var(--theme-darker)',
        },
        '& + $track': {
            backgroundColor: 'var(--foreground-disabled)',
        },
    },
    checked: {},
    track: {},
})(Switch);

export default ThemedSwitch;