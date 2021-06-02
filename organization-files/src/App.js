import './App.css';
import '@fontsource/roboto';
import MyTable from './Components/MyTable';
import { Typography } from '@material-ui/core';
import Options from './Components/Options';

function App() {
  return (
    <div className="App">
      <Typography variant="h1" component="h2">
        VAR Incidents
      </Typography>
      <Options/>
      <MyTable/>
    </div>
  );
}

export default App;
