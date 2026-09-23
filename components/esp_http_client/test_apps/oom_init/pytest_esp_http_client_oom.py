# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.qemu
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_esp_http_client_init_oom(dut: Dut) -> None:
    dut.expect(r'esp_http_client_init http OOM: before=(\d+) after=\1 delta=0')
    dut.expect(r'esp_http_client_init https OOM: before=(\d+) after=\1 delta=0')
    dut.expect_exact('esp_http_client_init OOM: PASS')
