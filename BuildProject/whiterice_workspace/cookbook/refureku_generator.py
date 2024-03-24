import whiterice

import pyutils.simplelogger as logger


class RefurekuGenerator(whiterice.ITable):
    def make(self, cook: whiterice.Cook) -> int:
        result = cook.exec.execute_straight(
            cook.config_ext.refureku_generator,
            cook.config_ext.refureku_generator_setting,
            work_dir=cook.config.project,
        )

        # 检查输出结果中是否包含 "error:"
        has_out_error = "error:" in result.out_str
        has_error = "error:" in result.error
        if has_out_error or has_error:
            whiterice.exit(
                whiterice.ERROR.MAKE_FAILED,
                result.error if has_error else result.out_str,
            )
        else:
            logger.info("未检测到错误信息，视为成功。")
        return super().make(cook)
