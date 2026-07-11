import json
import re
import sys
import uuid
from datetime import datetime, timedelta, timezone


def write(frame):
    print(json.dumps(frame, ensure_ascii=False, separators=(",", ":")), flush=True)


def output(text):
    return {"kind": "text", "text": text}


def waiting(prompt):
    expires = datetime.now(timezone.utc) + timedelta(seconds=300)
    return {
        "status": "waiting",
        "conversation_id": "weather-" + uuid.uuid4().hex,
        "expires_at": expires.isoformat().replace("+00:00", "Z"),
        "outputs": [output(prompt)],
        "consume": True,
    }


def completed(text):
    return {
        "status": "completed",
        "outputs": [output(text)],
        "consume": True,
    }


def message_text(event):
    message = event.get("message") or {}
    intent = str(message.get("intent_text") or "").strip()
    if intent:
        return intent
    return "".join(
        str(segment.get("text") or "")
        for segment in message.get("segments") or []
        if segment.get("type") == "text"
    ).strip()


def handle_event(params):
    text = message_text(params.get("event") or {})
    if params.get("continuation"):
        if text != "上海":
            return waiting("请发送正确的城市，目前仅支持上海。")
        return completed("上海城市天气：晴，25°C（测试数据）。")

    match = re.fullmatch(r"查询天气\s*(.*)", text)
    city = match.group(1).strip() if match else ""
    if not city:
        return waiting("请输入城市。")
    if city != "上海":
        return waiting("请发送正确的城市，目前仅支持上海。")
    return completed("上海天气：晴，25°C（测试数据）。")


for line in sys.stdin:
    frame = json.loads(line)
    if frame.get("type") == "event":
        continue
    if frame.get("type") != "request":
        continue

    request_id = frame.get("id")
    method = frame.get("method")
    try:
        if method == "system.init":
            result = {}
        elif method == "event.handle":
            result = handle_event(frame.get("params") or {})
        elif method == "system.shutdown":
            write({"type": "response", "id": request_id, "ok": True, "result": {}})
            break
        else:
            raise ValueError(f"unsupported method: {method}")
        write({"type": "response", "id": request_id, "ok": True, "result": result})
    except Exception as error:
        write({"type": "response", "id": request_id, "ok": False, "error": str(error)})
