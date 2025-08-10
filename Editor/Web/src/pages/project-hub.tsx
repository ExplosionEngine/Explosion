import { Tabs, Tab } from '@heroui/tabs';
import { User } from '@heroui/user';
import { Form } from '@heroui/form';
import { Button } from '@heroui/button';
import { Input } from '@heroui/input';

export default function ProjectHubPage() {
  return (
    <div className='h-screen p-6'>
      <div className='mb-4'>
        <User
          avatarProps={{
            src: '/logo.png',
          }}
          description='v0.0.1'
          name='Explosion Game Engine'
        />
      </div>

      <Tabs isVertical={true}>
        <Tab title='New Project' className='w-full pr-6'>
          <Form className='w-full ml-4'>
            <Input fullWidth isRequired label='Project Name' labelPlacement='outside' placeholder='HelloExplosion'/>
            <Input fullWidth isRequired label='Project Description' labelPlacement='outside' placeholder='A simple explosion game project.'/>
            <Input fullWidth isRequired label='Project Path' labelPlacement='outside' placeholder='/path/to/your/project'/>
            <Button color='primary'>Create</Button>
          </Form>
        </Tab>
        <Tab title='Recently Projects'>
          <div>TODO</div>
        </Tab>
      </Tabs>
    </div>
  );
};
