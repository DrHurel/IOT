import os
import os.path
import questionary
import re
import getpass
import termcolor
import datetime


class ClassCreator:
    def __init__(self):
        self._repository_root = os.path.abspath(
            os.path.join(os.path.dirname(os.path.realpath(__file__)), "..", "..")
        )
        self._templates_folder = os.path.join(
            self._repository_root, "Devtools", "CodeTemplates"
        )
        self._src_folder = os.path.join(self._repository_root, "src")

    def _get_templates(self):
        templates = os.listdir(self._templates_folder)
        templates.sort()
        return templates

    @staticmethod
    def _is_classname_valid(classname: str) -> bool:
        return re.match(r"^[a-zA-Z\_][a-zA-Z\_0-9]*$", classname) is not None

    def _is_destination_folder_valid(self, folder: str):
        f = folder.strip()
        return f.lower() == f and len(f) != 0

    def _get_destination_folder(self):
        path = os.getcwd()
        choices = {
            "Application": "apps",
            "Library": "libs",
        }
        selection = questionary.select(
            "In which module do you want to create the class",
            choices=list(choices.keys()),
        ).ask()
        subfolder = choices[selection]
        os.chdir(os.path.join(self._src_folder, subfolder))
        while True:
            dest_folder = os.path.join(
                subfolder,
                questionary.path(
                    "Select the destination folder",
                    only_directories=True,
                    validate=self._is_destination_folder_valid,
                ).ask(),
            )
            if questionary.confirm(
                "Do you really want to create the class in {}?".format(dest_folder)
            ).ask():
                os.chdir(path)
                return dest_folder

    @staticmethod
    def _is_namespace_valid( namespace: str) -> bool:
        return re.search(r"^[a-z]\w*(\:\:[a-z]\w*)*$", namespace) is not None

    def _create_destination_folder(self, destination_folder: str):
        dest = os.path.join(self._src_folder, destination_folder)
        os.makedirs(dest, exist_ok=True)
        if not questionary.confirm(
            "Do you want to create a new namespace for this folder?"
        ).ask():
            return
        default_namespace: str = (
            self._get_namespace(destination_folder) + "::" + os.path.basename(dest)
        )
        namespace = questionary.text(
            "New namespace:",
            default=default_namespace,
            validate=ClassCreator._is_namespace_valid,
        ).ask()
        with open(os.path.join(dest, "NAMESPACE"), "w") as file:
            file.write(namespace)

    def _get_namespace(self, destination_folder: str) -> str:
        # Base case: stop recursion if destination_folder is empty or root
        if not destination_folder or destination_folder == os.path.sep:
            return "um"
        if os.path.isdir(
            os.path.join(self._src_folder, destination_folder, "Devtools")
        ):  # no NAMESPACE file found in all the folders
            return "um"
        namespace_file = os.path.join(self._src_folder, destination_folder, "NAMESPACE")
        if not os.path.isfile(namespace_file):
            return self._get_namespace(os.path.dirname(destination_folder))
        with open(namespace_file) as file:
            return file.readline().strip()

    def _convert_case(self, match_obj: re.Match[str]) -> str:
        if match_obj.group(1) is not None:
            return "_" + match_obj.group(1).lower()
        return ""

    def _get_class_lower_case(self, class_name: str) -> str:
        first_later = class_name[:1].lower()

        return (
            first_later
            + re.sub(r"([A-Z][a-z]+)", self._convert_case, class_name[1:]).lower()
        )

    def _generate_dictionary(self, destination_folder: str, class_name: str):
        return {
            "CLASS_NAME": class_name,
            "CLASS_NAME_LOWER": self._get_class_lower_case(class_name),
            "FILENAME_BASE": class_name,
            "AUTHOR": getpass.getuser(),
            "DATE": datetime.datetime.now().strftime("%Y-%m-%d"),
            "NAMESPACE": self._get_namespace(destination_folder),
        }

    def _process_file(
        self, template_file: str, destination_folder: str, dictionary: dict[str, str]
    ) -> str:
        with open(template_file) as template:
            content = template.read()
            for src, dest in dictionary.items():
                content = content.replace("${}$".format(src), dest)
            destination_file = os.path.join(
                self._src_folder,
                destination_folder,
                dictionary["FILENAME_BASE"] + os.path.splitext(template_file)[1],
            )
            with open(destination_file, "w") as dest:
                dest.write(content)
                return destination_file

    def _create_class(self, destination_folder: str, template: str, class_name: str):
        if not questionary.confirm(
            "Do you really want to create '{}' class based on '{}' template in {} folder?".format(
                class_name, template, destination_folder
            )
        ).ask():
            return
        if not os.path.isdir(os.path.join(self._src_folder, destination_folder)):
            if not questionary.confirm(
                "'{}' folder does not exist. Do you want to create it?".format(
                    destination_folder
                )
            ).ask():
                return
            self._create_destination_folder(destination_folder)
        dictionary = self._generate_dictionary(destination_folder, class_name)
        template_folder = os.path.join(self._templates_folder, template)
        for file in os.listdir(template_folder):
            result_file = self._process_file(
                os.path.join(template_folder, file), destination_folder, dictionary
            )
            if result_file.endswith(".cpp"):
                self._patch_cmakelists(result_file)
        termcolor.cprint(class_name + " created", "black", "on_green")

    def _get_cpp_files(self, folder: str) -> list[str]:
        cpp_files: list[str] = []
        for root, _, files in os.walk(folder):
            for file in files:
                if file.endswith(".cpp"):
                    relative_path = os.path.relpath(root, folder)
                    cpp_files.append(
                        file
                        if relative_path == "."
                        else os.path.join(relative_path, file)
                    )
        cpp_files.sort()
        return cpp_files

    def _patch_cmakelists(self, file: str):
        _dir = os.path.dirname(file)
        if "src/libs/" in file:
            is_lib = True
            regex = r"([\s\S]*)(add_library\(([\w\-_]*)\s*(\w*))([^\)]*)\)([\s\S]*)"
        elif "src/apps/" in file:
            is_lib = False
            regex = r"([\s\S]*)(add_executable\(([\w\-_]*))([^\)]*)\)([\s\S]*)"
        elif "src/testing/unit_tests" in file:
            # for unit tests, the executable are automatically generated
            # no need to patch the CMakeLists.txt
            return
        else:
            assert False  # Not yet supported
        while _dir != self._src_folder:
            cmakelists_file = os.path.join(_dir, "CMakeLists.txt")
            if not os.path.isfile(cmakelists_file):
                _dir = os.path.dirname(_dir)
                continue
            with open(cmakelists_file) as fd:
                content = fd.read()
            match = re.search(regex, content)
            assert match
            groups = match.groups()
            cpp_files = self._get_cpp_files(_dir)
            if is_lib:
                content = (
                    groups[0]
                    + groups[1]
                    + "\n\t"
                    + "\n\t".join(cpp_files)
                    + "\n)"
                    + groups[5]
                )
            else:
                content = (
                    groups[0]
                    + groups[1]
                    + "\n\t"
                    + "\n\t".join(cpp_files)
                    + "\n)"
                    + groups[4]
                )
            with open(cmakelists_file, "w") as fd:
                fd.write(content)
                return
        assert False  # No CMakeLists.txt found

    def create(self):
        while True:
            dest_folder = self._get_destination_folder()
            template = questionary.select(
                "Select a template", self._get_templates()
            ).ask()
            class_name = questionary.text(
                "Enter the new class name", validate=self._is_classname_valid
            ).ask()
            self._create_class(dest_folder, template, class_name)
            if not questionary.confirm("Do you want to create another class?").ask():
                return


if __name__ == "__main__":
    creator = ClassCreator()
    creator.create()
