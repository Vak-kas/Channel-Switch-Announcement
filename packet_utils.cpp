#include "packet_utils.h"

u_char* inject_data_into_packet(
    const u_char* packet,
    int total_packet_len,
    const std::vector<InsertData>& inserts,
    size_t& new_packet_len
)
{
    // 정렬(삽입 순서 기준)
    std::vector<InsertData> sorted(inserts);
    std::sort(sorted.begin(), sorted.end(),
        [](const InsertData& a, const InsertData& b) {
            return a.pos < b.pos;
        });

    // 새 패킷 길이 계산
    new_packet_len = total_packet_len;
    for (size_t i = 0; i < sorted.size(); i++)
    {
        new_packet_len += sorted[i].len;
    }

    // 새 패킷 버퍼 할당
    u_char* new_packet = new u_char[new_packet_len];

    const uint8_t* src = packet;   // 원본 기준
    uint8_t* dst = new_packet;     // 새 버퍼

    // 삽입 및 복사
    for (size_t i = 0; i < sorted.size(); ++i)
    {
        const InsertData& insert = sorted[i];

        // 안전 체크
        if (insert.pos < packet || insert.pos > packet + total_packet_len)
            continue;

        size_t chunk_len = insert.pos - src;

        // 앞부분 복사
        memcpy(dst, src, chunk_len);
        dst += chunk_len;

        // 삽입
        memcpy(dst, insert.data, insert.len);
        dst += insert.len;

        // src 이동
        src = insert.pos;
    }

    // ===== 5. 나머지 복사 =====
    size_t remain_len = (packet + total_packet_len) - src;
    memcpy(dst, src, remain_len);

    return new_packet;
}


void write_data_to_packet(u_char* packet, size_t offset, const uint8_t* data, size_t len)
{
    memcpy(packet + offset, data, len);
}