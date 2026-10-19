/**
 * BouffaloSDK RESTful API Web Interface
 * GPIO 控制操作的前端 JavaScript
 */

document.addEventListener('DOMContentLoaded', function() {
    // 获取 DOM 元素
    const getBtn = document.getElementById('getBtn');
    const postBtn = document.getElementById('postBtn');
    const putBtn = document.getElementById('putBtn');
    const patchBtn = document.getElementById('patchBtn');
    const deleteBtn = document.getElementById('deleteBtn');
    const postInputGroup = document.getElementById('postInputGroup');
    const patchInputGroup = document.getElementById('patchInputGroup');
    const putInputGroup = document.getElementById('putInputGroup');
    const deleteInputGroup = document.getElementById('deleteInputGroup');
    const responseOutput = document.getElementById('responseOutput');

    // API 基础 URL
    const API_BASE_URL = '';

    // 显示响应结果
    function displayResponse(data, status) {
        const timestamp = new Date().toLocaleTimeString();
        let formattedResponse = `[${timestamp}] HTTP Status: ${status}\n\n`;

        try {
            // 尝试解析 JSON
            const jsonData = JSON.parse(data);
            formattedResponse += JSON.stringify(jsonData, null, 2);
        } catch (e) {
            // 如果不是 JSON，直接显示原始内容
            formattedResponse += data;
        }

        responseOutput.textContent = formattedResponse;
        responseOutput.style.color = status >= 200 && status < 300 ? '#2ecc71' : '#e74c3c';
    }

    // 显示加载状态
    function showLoading(button) {
        const originalText = button.innerHTML;
        button.innerHTML = '<span class="btn-icon">⏳</span><span class="btn-text">处理中...</span>';
        button.disabled = true;

        setTimeout(() => {
            button.innerHTML = originalText;
            button.disabled = false;
        }, 2000);
    }

    // GET 请求 - 获取所有 GPIO
    async function fetchGpios() {
        try {
            const response = await fetch(`${API_BASE_URL}/api/v1/gpios`, {
                method: 'GET',
                headers: {
                    'Content-Type': 'application/json'
                }
            });

            const data = await response.text();
            displayResponse(data, response.status);
        } catch (error) {
            displayResponse(`Error: ${error.message}`, 500);
        }
    }

    // POST 请求 - 创建 GPIO
    async function createGpio() {
        const pin = document.getElementById('postPin').value;
        const value = document.getElementById('postValue').value;

        if (!pin || pin < 1 || pin > 6) {
            displayResponse('Error: 请输入有效的引脚编号 (1-6)', 400);
            return;
        }

        try {
            const response = await fetch(`${API_BASE_URL}/api/v1/gpios`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    pin: parseInt(pin),
                    value: parseInt(value)
                })
            });

            const data = await response.text();
            displayResponse(data, response.status);
        } catch (error) {
            displayResponse(`Error: ${error.message}`, 500);
        }
    }

    // PATCH 请求 - 部分更新 GPIO
    async function updateGpio() {
        const pin = document.getElementById('patchPin').value;
        const value = document.getElementById('patchValue').value;

        if (!pin || pin < 1 || pin > 6) {
            displayResponse('Error: 请输入有效的引脚编号 (1-6)', 400);
            return;
        }

        try {
            const response = await fetch(`${API_BASE_URL}/api/v1/gpios/${pin}`, {
                method: 'PATCH',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    value: parseInt(value)
                })
            });

            const data = await response.text();
            displayResponse(data, response.status);
        } catch (error) {
            displayResponse(`Error: ${error.message}`, 500);
        }
    }

    // PUT 请求 - 完整更新 GPIO
    async function putGpio() {
        const pin = document.getElementById('putPin').value;
        const value = document.getElementById('putValue').value;

        if (!pin || pin < 1 || pin > 6) {
            displayResponse('Error: 请输入有效的引脚编号 (1-6)', 400);
            return;
        }

        try {
            const response = await fetch(`${API_BASE_URL}/api/v1/gpios/${pin}`, {
                method: 'PUT',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    value: parseInt(value)
                })
            });

            const data = await response.text();
            displayResponse(data, response.status);
        } catch (error) {
            displayResponse(`Error: ${error.message}`, 500);
        }
    }

    // DELETE 请求 - 删除 GPIO
    async function deleteGpio() {
        const pin = document.getElementById('deletePin').value;

        if (!pin || pin < 1 || pin > 6) {
            displayResponse('Error: 请输入有效的引脚编号 (1-6)', 400);
            return;
        }

        try {
            const response = await fetch(`${API_BASE_URL}/api/v1/gpios/${pin}`, {
                method: 'DELETE',
                headers: {
                    'Content-Type': 'application/json'
                }
            });

            const data = await response.text();
            displayResponse(data, response.status);
        } catch (error) {
            displayResponse(`Error: ${error.message}`, 500);
        }
    }

    // 显示/隐藏输入组
    function showInputGroup(group) {
        if (group === 'post') {
            postInputGroup.style.display = postInputGroup.style.display === 'none' ? 'block' : 'none';
            patchInputGroup.style.display = 'none';
            putInputGroup.style.display = 'none';
            deleteInputGroup.style.display = 'none';
        } else if (group === 'patch') {
            patchInputGroup.style.display = patchInputGroup.style.display === 'none' ? 'block' : 'none';
            postInputGroup.style.display = 'none';
            putInputGroup.style.display = 'none';
            deleteInputGroup.style.display = 'none';
        } else if (group === 'put') {
            putInputGroup.style.display = putInputGroup.style.display === 'none' ? 'block' : 'none';
            postInputGroup.style.display = 'none';
            patchInputGroup.style.display = 'none';
            deleteInputGroup.style.display = 'none';
        } else if (group === 'delete') {
            deleteInputGroup.style.display = deleteInputGroup.style.display === 'none' ? 'block' : 'none';
            postInputGroup.style.display = 'none';
            patchInputGroup.style.display = 'none';
            putInputGroup.style.display = 'none';
        }
    }

    // 绑定按钮事件
    getBtn.addEventListener('click', function() {
        showLoading(this);
        fetchGpios();
        postInputGroup.style.display = 'none';
        patchInputGroup.style.display = 'none';
        putInputGroup.style.display = 'none';
        deleteInputGroup.style.display = 'none';
    });

    postBtn.addEventListener('click', function() {
        showLoading(this);
        showInputGroup('post');
        // 延迟执行 POST 请求以显示输入组
        setTimeout(createGpio, 100);
    });

    putBtn.addEventListener('click', function() {
        showLoading(this);
        showInputGroup('put');
        // 延迟执行 PUT 请求以显示输入组
        setTimeout(putGpio, 100);
    });

    patchBtn.addEventListener('click', function() {
        showLoading(this);
        showInputGroup('patch');
        // 延迟执行 PATCH 请求以显示输入组
        setTimeout(updateGpio, 100);
    });

    deleteBtn.addEventListener('click', function() {
        showLoading(this);
        showInputGroup('delete');
        // 延迟执行 DELETE 请求以显示输入组
        setTimeout(deleteGpio, 100);
    });

    // 初始化：显示欢迎信息
    const welcomeMessage = `
欢迎使用 BouffaloSDK RESTful API 测试平台！

可用操作：
1. GET /api/v1/gpios    - 获取所有 GPIO 状态
2. POST /api/v1/gpios   - 创建新的 GPIO
3. PUT /api/v1/gpios    - 完整更新指定 GPIO
4. PATCH /api/v1/gpios  - 部分更新指定 GPIO
5. DELETE /api/v1/gpios - 删除指定 GPIO

请点击上方按钮开始测试...
    `;

    displayResponse(welcomeMessage, 200);
    console.log('BouffaloSDK RESTful API Web Interface 初始化完成');
});
