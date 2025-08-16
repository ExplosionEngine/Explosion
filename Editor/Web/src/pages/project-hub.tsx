import { useEffect, useState } from 'react';
import { QWebChannel } from '@/qwebchannel'
import { Tabs, Tab } from '@heroui/tabs';
import { User } from '@heroui/user';
import { Form } from '@heroui/form';
import { Button } from '@heroui/button';
import { Input } from '@heroui/input';
import { Chip } from '@heroui/chip';
import { Listbox, ListboxItem } from '@heroui/listbox';
import { Avatar } from '@heroui/avatar';
import { ScrollShadow } from "@heroui/scroll-shadow";

export default function ProjectHubPage() {
  const [engineVersion, setEngineVersion] = useState('');
  // TODO fetch from c++
  const [recentlyProjects] = useState([{
    key: '1',
    name: 'HelloExplosion1',
    icon: '/logo.png',
    path: '/path/to/HelloExplosion'
  }, {
    key: '2',
    name: 'HelloExplosion2',
    icon: '/logo.png',
    path: '/path/to/HelloExplosion'
  }, {
    key: '3',
    name: 'HelloExplosion3',
    icon: '/logo.png',
    path: '/path/to/HelloExplosion'
  }, {
    key: '4',
    name: 'HelloExplosion4',
    icon: '/logo.png',
    path: '/path/to/HelloExplosion'
  }, {
    key: '5',
    name: 'HelloExplosion5',
    icon: '/logo.png',
    path: '/path/to/HelloExplosion'
  }, {
    key: '6',
    name: 'HelloExplosion6',
    icon: '/logo.png',
    path: '/path/to/HelloExplosion'
  }, {
    key: '7',
    name: 'HelloExplosion7',
    icon: '/logo.png',
    path: '/path/to/HelloExplosion'
  }, {
    key: '8',
    name: 'HelloExplosion8',
    icon: '/logo.png',
    path: '/path/to/HelloExplosion'
  }, {
    key: '9',
    name: 'HelloExplosion9',
    icon: '/logo.png',
    path: '/path/to/HelloExplosion'
  }, {
    key: '10',
    name: 'HelloExplosion10',
    icon: '/logo.png',
    path: '/path/to/HelloExplosion'
  }, {
    key: '11',
    name: 'HelloExplosion11',
    icon: '/logo.png',
    path: '/path/to/HelloExplosion'
  }, {
    key: '12',
    name: 'HelloExplosion12',
    icon: '/logo.png',
    path: '/path/to/HelloExplosion'
  }, {
    key: '13',
    name: 'HelloExplosion13',
    icon: '/logo.png',
    path: '/path/to/HelloExplosion'
  }]);

  useEffect(() => {
    new QWebChannel(window.qt.webChannelTransport, (channel: QWebChannel) : void => {
      window.bridge = channel.objects.bridge;
      setEngineVersion(window.bridge.engineVersion);
    })
  }, []);

  function onCreateProject(): void
  {
    window.bridge.CreateProject();
  }

  return (
    <div className='h-screen p-6'>
      <div className='mb-4'>
        <User
          avatarProps={{
            src: '/logo.png'
          }}
          description={
            <div className='mt-1'>
              <Chip className='ml-1' size='sm' color='secondary' variant='flat'>{engineVersion}</Chip>
            </div>
          }
          name='Explosion Game Engine'/>
      </div>

      <Tabs isVertical={true}>
        <Tab title='Recently Projects' className='w-full pr-6'>
          <ScrollShadow
            hideScrollBar
            className='h-[450px]'
            size={60}>
            <Listbox
              items={recentlyProjects}
              variant='flat'>
              {(item) => (
                <ListboxItem key={item.key} textValue={item.name}>
                  <div className='flex gap-2 items-center'>
                    <Avatar alt={item.name} className='shrink-0' size='sm' src={item.icon} />
                    <div className='flex flex-col'>
                      <span className='text-small'>{item.name}</span>
                      <span className='text-tiny text-default-400'>{item.path}</span>
                    </div>
                  </div>
                </ListboxItem>
              )}
            </Listbox>
          </ScrollShadow>
        </Tab>
        <Tab title='New Project' className='w-full pr-6'>
          <Form className='w-full ml-4'>
            <Input fullWidth isRequired label='Project Name' labelPlacement='outside' placeholder='HelloExplosion'/>
            <Input fullWidth isRequired label='Project Description' labelPlacement='outside' placeholder='A simple explosion game project.'/>
            <Input fullWidth isRequired label='Project Path' labelPlacement='outside' placeholder='/path/to/your/project'/>
            <Button color='primary' onPress={onCreateProject}>Create</Button>
          </Form>
        </Tab>
      </Tabs>
    </div>
  );
};
