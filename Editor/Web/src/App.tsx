import { Route, Routes } from 'react-router-dom';
import ProjectHubPage from '@/pages/project-hub';

function App() {
  return (
    <Routes>
      <Route element={<ProjectHubPage/>} path='/project-hub'/>
    </Routes>
  );
}

export default App;
