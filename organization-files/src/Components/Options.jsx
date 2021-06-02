import React from 'react';
import { makeStyles } from '@material-ui/core/styles';
import Methods from './Methods';
import SimpleSelect from './SimpleSelect';
import MySwitch from './MySwitch';

const useStyles = makeStyles((theme) => ({
  root: {
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'center',
    '& > *': {
      margin: theme.spacing(1),
    },
  },
}));

export default function Options() {
    const classes = useStyles();
    return(
        <div className={classes.root}>
            <MySwitch/>
            <Methods/>
        </div>
    );
}