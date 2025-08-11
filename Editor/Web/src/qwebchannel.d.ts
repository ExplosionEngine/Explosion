export { QWebChannel } from './qwebchannel.js';

declare global {
  interface Window {
    qt: any;
    bridge: any;
  }
}
