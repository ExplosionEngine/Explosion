import { useEffect, useState } from 'react';
import { QWebChannel } from '@/qwebchannel'
import { Tabs, Tab } from '@heroui/tabs';
import { User } from '@heroui/user';
import { Form } from '@heroui/form';
import {Button, PressEvent} from '@heroui/button';
import { Input } from '@heroui/input';
import { Chip } from '@heroui/chip';
import { Listbox, ListboxItem } from '@heroui/listbox';
import { Avatar } from '@heroui/avatar';
import { ScrollShadow } from "@heroui/scroll-shadow";
import { Select, SelectItem } from '@heroui/react';

interface RecentProjectInfo {
  name: string;
  path: string;
}

interface ProjectTemplateInfo {
  name: string;
  path: string;
}

export default function ProjectHubPage() {
  const [engineVersion, setEngineVersion] = useState('');
  const [recentProjects, setRecentProjects] = useState(Array<RecentProjectInfo>);
  const [projectTemplates, setProjectTemplates] = useState(Array<ProjectTemplateInfo>);

  useEffect(() => {
    new QWebChannel(window.qt.webChannelTransport, (channel: QWebChannel) : void => {
      window.backend = channel.objects.backend;
      setEngineVersion(window.backend.engineVersion);
      setRecentProjects(window.backend.recentProjects);
      setProjectTemplates(window.backend.projectTemplates);
    })
  }, []);

  function onCreateProject() : void
  {
    window.backend.CreateProject();
  }

  function onOpenProject(e: PressEvent) : void
  {
    // TODO
    const index = parseInt(e.target.getAttribute('data-key') as string);
    console.error('onOpenProject:', index);
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
              items={recentProjects}
              variant='flat'>
              {recentProjects.map((item, i) => (
                  <ListboxItem key={i} onPress={onOpenProject} textValue={item.name}>
                    <div className='flex gap-2 items-center'>
                      <Avatar alt={item.name} className='shrink-0' size='sm' name={item.name} />
                      <div className='flex flex-col'>
                        <span className='text-small'>{item.name}</span>
                        <span className='text-tiny text-default-400'>{item.path}</span>
                      </div>
                    </div>
                  </ListboxItem>
              ))}
            </Listbox>
          </ScrollShadow>
        </Tab>
        <Tab title='New Project' className='w-full pr-6'>
          <Form className='w-full ml-4'>
            <Input fullWidth isRequired label='Project Name' labelPlacement='outside' placeholder='HelloExplosion'/>
            <Input fullWidth isRequired label='Project Path' labelPlacement='outside' placeholder='/path/to/your/project'/>
            <Select fullWidth isRequired label='Project Template' labelPlacement='outside' defaultSelectedKeys={['0']}>
              {projectTemplates.map((item, i) => (
                <SelectItem key={i}>{item.name}</SelectItem>
              ))}
            </Select>
            <Button color='primary' onPress={onCreateProject}>Create</Button>
          </Form>
        </Tab>
      </Tabs>
    </div>
  );
};
