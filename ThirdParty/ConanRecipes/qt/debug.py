from conan.api.conan_api import ConanAPI
from conan.cli.cli import Cli

if __name__ == '__main__':
    api = ConanAPI()
    api.command = Cli(api)
    api.command.run(["build", ".\conanfile.py", "--version=\"6.10.1-exp\""])
