import { Route, Routes } from 'react-router-dom';
import ProjectHubPage from '@/pages/project-hub';
import PrototypePage from '@/pages/prototype';

function App() {
  return (
    <Routes>
      <Route element={<ProjectHubPage />} path='/project-hub' />
      <Route element={<PrototypePage />} path='/prototype' />
    </Routes>
  );
}

export default App;
