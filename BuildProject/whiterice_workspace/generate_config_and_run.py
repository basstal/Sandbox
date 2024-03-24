import json
import os
import pathlib
from whiterice.tools.generate_config_sandbox import GenerateConfigSandbox


def generate_config():
    generator = GenerateConfigSandbox()
    argparser = generator.create_argparser()
    args = argparser.parse_args()
    workspace = os.path.dirname(__file__)
    content, output_filename = GenerateConfigSandbox.generate_config_common_args(args)
    whiterice_extensions = content["optional"]["whiterice_extensions"] or {}
    content["optional"]["whiterice_extensions"] = whiterice_extensions
    project = str(pathlib.Path(workspace).parent.parent)
    content["project"] = project
    whiterice_extensions["enabled"] = True
    whiterice_extensions["workspace"] = workspace
    whiterice_extensions["config_proto"] = os.path.join(workspace, "config.proto")
    whiterice_extensions["config_proto_name"] = "WhitericeExtensions"
    extension_config_json = os.path.join(workspace, "config_workspace.json")
    whiterice_extensions["config_file"] = extension_config_json

    extension_config = {
        # "copy_dlls": [],
        "refureku_generator": os.path.join(
            project, "Plugins/refureku/Refureku/build/Bin/Debug/RefurekuGenerator.exe"
        ),  # 仅支持 windows
        "refureku_generator_setting": os.path.join(
            project, "Plugins/refureku/RefurekuSettings.toml"
        ),
    }
    with open(extension_config_json, "w+") as f:
        json.dump(extension_config, f)
    output_config = os.path.join(workspace, f"{output_filename}.json")
    with open(output_config, "w+") as f:
        json.dump(content, f)
    return output_config


def run():
    config_file = generate_config()
    from pyutils.executor import Executor

    exec = Executor(True)
    result = exec.execute_straight(
        "python",
        [
            "-m",
            "whiterice",
            "--config",
            config_file,
            "--backend=sandbox",
            "--verbose",
            #  "--overview"
        ],
        work_dir=os.path.dirname(__file__),
        ignore_error=True,
    )
    if result.code != 0:
        import pyutils.simplelogger as logger

        logger.info(result.out_str)
        logger.error(result.error)


if __name__ == "__main__":
    run()
