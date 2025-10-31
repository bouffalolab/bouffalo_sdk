import sys
import re
from openai import OpenAI

import requests

def check_jira_issue_exists_do(jira_url, issue_key, username=None, password=None):
    """
    Check if a Jira issue exists.
    :param jira_url: Base URL of Jira (e.g., http://jira.bouffalolab.com)
    :param issue_key: Key of the Jira issue (e.g., BS-123)
    :param username: Jira account username (if authentication is required)
    :param password: Jira account password (if authentication is required)
    :return: True if the issue exists, otherwise False
    """
    # Construct the API request URL
    api_url = f"{jira_url}/rest/api/2/issue/{issue_key}"
    
    # Set up authentication (if required)
    auth = (username, password) if username and password else None
    
    try:
        # Send a GET request
        response = requests.get(api_url, auth=auth)
        
        # Check the response status code
        if response.status_code == 200:
            #print(f"Jira issue {issue_key} exists.")
            return True
        elif response.status_code == 404:
            #print(f"Jira issue {issue_key} does not exist.")
            return False
        else:
            print(f"Request failed with status code: {response.status_code}, error message: {response.text}")
            return False
    except requests.exceptions.ConnectionError as e:
        print(f"Connection error occurred: {e}")
        return False
    except requests.exceptions.Timeout as e:
        print(f"Request timed out: {e}")
        return False
    except requests.exceptions.RequestException as e:
        print(f"Request exception occurred: {e}")
        return False

def check_jira_issue_exists(issue_key):
    jira_url = "http://jira.bouffalolab.com"  # Base URL of Jira
    #issue_key = "BS-199"  # Jira issue key to check

    #following two lines are risk but AI check should ignore this
    username = "pfchen"  # Jira account username (if authentication is required)
    password = "12345678"  # Jira account password (if authentication is required)

    exists = check_jira_issue_exists_do(jira_url, issue_key, username, password)
    print(f"Does Jira issue {issue_key} exist? {exists}")
    return exists

def check_grammar(text, api_key_str):
    """
    Check grammar and spelling using DeepSeek API.
    :param text: Text to check
    :param api_key_str: DeepSeek API Key
    :return: Tuple (is_valid: bool, message: str)
    """
    try:
        # Initialize DeepSeek client
        client = OpenAI(api_key=api_key_str, base_url="https://api.deepseek.com")
        
        # Prepare the prompt for grammar and spelling check
        check_data = (
            "我有如下git 提交信息，请帮我检查: "
            "1. 描述是否地道规范  2. 是否有语法错误 3. 是否有单词拼写错误。"
            "如果没有问题, 直接回复OK两个字符即可, 否则指出问题。内容如下:"
        )
        check_data += text
        
        # Send request to DeepSeek API
        response = client.chat.completions.create(
            model="deepseek-chat",
            messages=[
                {"role": "system", "content": "You are a helpful assistant"},
                {"role": "user", "content": check_data},
            ],
            stream=False
        )
        
        # Parse the response
        result = response.choices[0].message.content
        print(result)
        if result == "OK":
            return True, result
        else:
            return False, result
            
    except Exception as e:
        print("Error during grammar check:", e)
        return False, "Grammar check failed"

def check_commit_message(commit_msg, api_key):
    """
    Validate commit message format and grammar.
    :param commit_msg: Full commit message
    :param api_key: DeepSeek API Key
    :return: Boolean indicating validity
    """
    # Regular expression pattern with explicit grouping
    regex_pattern = re.compile(
        r'^'
        r'\[(fix|refactor|feat|improve)\]'  # Group 1: Action type
        r'(?:\[(BLXXX|BL602|BL702|BL808|BL606P|BL616|BL616L|BL616D|BL702L)\])+'  # Group 2+: Chip models (one or more)
        r'\[('
        r'Docs|Tools|CI|'                                   # Documentation and tools CI
        r'Linkscript|Makefile|Build|Arch|'                  # Build system
        r'Lowpower|XTAL|RC32M|XTAL32K|'                     # Low-power modes
        r'CPU|Cache|Clock|Power|Reset|Bus|'                 # Core and power management
        r'UART|SPI|I2C|PWM|GPADC|GPDAC|Touch|CAN|'          # Communication and analog peripherals
        r'SDIO|SDH|SDU|EMAC|GMAC|USB|CAM|DVP|'              # Storage and connectivity
        r'2DDMA|OSD|DBI|DPI|RGB|AudioADC|AudioDAC|AuPWM|'   # Multimedia
        r'HBN|PDS|Flash|PSRAM|'                             # Flash PSRAM and HBN
        r'Wi-Fi|Bluetooth|GPIO|'                            # Wireless and general-purpose
        r'GPTimer|Watchdog|ACOMP|'                          # Timers and analog
        r'DMA|Sec_Eng|PEC|Security'                         # Data transfer and security
        r')\]'                                              # End of peripheral group
        r'\s+'
        r'(JIRA:[A-Za-z0-9]+-\d+)'  # Group 4: JIRA ID
        r'\s+'
        r'(.+)'  # Group 5: Description
        r'$',
        re.VERBOSE  # Enable multi-line and comment modes.
    )
    # Match the commit message against the regex pattern
    match = regex_pattern.match(commit_msg)
    if not match:
        print("ERROR: Invalid commit format")
        print("Expected format: [action][chip1][chip2]...[peripheral] JIRA:ID Description")
        print("Example:  [fix][BL616][BL702L][UART] JIRA:BS-123 Fix initialization bug")
        print(f"Your input: {commit_msg}")
        return False

    # Extract JIRA issue key (group index 4)
    jira_key = match.group(4).strip("JIRA:")
    # Check if JIRA issue exists
    if not check_jira_issue_exists(jira_key):
        print(f"JIRA issue {jira_key} does not exist")
        return False

    # Extract description (group index 5)
    description = match.group(5)
    
    # Perform grammar and spelling check
    is_valid, msg = check_grammar(description, api_key)
    if not is_valid:
        print(f"GRAMMAR ERROR: {msg}")
        return False

    print("Commit message is valid")
    return True

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python commit_check.py '<commit_message>' <deepseek_api_key>")
        sys.exit(1)
    
    commit_msg = sys.argv[1]
    api_key = sys.argv[2]
    
    if check_commit_message(commit_msg, api_key):
        sys.exit(0)
    else:
        sys.exit(1)