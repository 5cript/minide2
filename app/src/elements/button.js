import Button from '@material-ui/core/Button';
import { styled } from '@material-ui/core/styles';

export const StyledButton = styled(Button)({
    borderRadius: '0px',
    padding: '5px',
    marginLeft: '3px',
    background: 'var(--theme-darker)',
    color: 'var(--foreground-color)',
    '&:hover': {
        background: 'var(--theme-color)'
    }
});

export const SlimButton = styled(Button)({
    borderRadius: '0px',
    padding: '5px',
    marginLeft: '3px',
    height: '38px',
    background: 'var(--theme-darker)',
    color: 'var(--foreground-color)',
    '&:hover': {
        background: 'var(--theme-color)'
    }
});

export default StyledButton;