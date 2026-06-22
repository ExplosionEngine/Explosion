import { useEffect, useState } from 'react';
import { Card, CardBody, CardHeader, Slider, Button, Chip } from '@heroui/react';
import { QWebChannel } from '@/qwebchannel';

const minRotationSpeed = -360;
const maxRotationSpeed = 360;

export default function PrototypePage() {
  const [ready, setReady] = useState(false);
  const [rotationSpeed, setRotationSpeed] = useState(90);

  useEffect(() => {
    new QWebChannel(window.qt.webChannelTransport, (channel: QWebChannel): void => {
      window.backend = channel.objects.backend;
      setRotationSpeed(window.backend.rotationSpeed);
      setReady(true);
    });
  }, []);

  function onRotationSpeedChange(value: number | number[]): void {
    const speed = Array.isArray(value) ? value[0] : value;
    setRotationSpeed(speed);
    if (ready) {
      window.backend.SetRotationSpeed(speed);
    }
  }

  return (
    <div className='h-screen p-6 flex flex-col gap-4'>
      <div className='flex items-center gap-2'>
        <span className='text-lg font-semibold'>Prototype Playground</span>
        <Chip color='secondary' size='sm' variant='flat'>
          native graphics + web
        </Chip>
      </div>

      <Card>
        <CardHeader className='flex justify-between'>
          <span className='text-medium'>Triangle Rotation</span>
          <Chip color='primary' size='sm' variant='flat'>
            {Math.round(rotationSpeed)} °/s
          </Chip>
        </CardHeader>
        <CardBody className='gap-4'>
          <Slider
            aria-label='Rotation Speed'
            label='Rotation Speed'
            getValue={(v) => `${v} °/s`}
            maxValue={maxRotationSpeed}
            minValue={minRotationSpeed}
            step={1}
            value={rotationSpeed}
            onChange={onRotationSpeedChange}
          />
          <div className='flex gap-2'>
            <Button size='sm' variant='flat' onPress={() => onRotationSpeedChange(0)}>
              Stop
            </Button>
            <Button size='sm' variant='flat' onPress={() => onRotationSpeedChange(90)}>
              Reset
            </Button>
            <Button size='sm' variant='flat' onPress={() => onRotationSpeedChange(-rotationSpeed)}>
              Reverse
            </Button>
          </div>
        </CardBody>
      </Card>

      <p className='text-tiny text-default-400'>
        The triangle on the left is rendered through the native graphics API (RHI). This panel is a web widget; both
        live inside the same Qt layout.
      </p>
    </div>
  );
}
