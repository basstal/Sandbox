import os
import shutil
import whiterice
from pyutils.fsext import get_files_glob


class CopyDlls(whiterice.ITable):
    @whiterice.priority(10)
    def build(self, cook: whiterice.Cook) -> int:
        import pyutils.simplelogger as logger

        bin_path = os.path.join(cook.config.project, "build/Bin")
        dlls = get_files_glob(bin_path, "*.dll", recursive=False)
        logger.info(f"dlls : {dlls}")
        for src in dlls:
            name_with_extension = os.path.split(src)[1]
            dst = os.path.join(bin_path, "Debug", name_with_extension)
            logger.info(f"copy from {src} to {dst}")
            shutil.copy2(
                src,
                # TODO:路径没有区分 release
                dst,
            )
        return super().build(cook)
