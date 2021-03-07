
#ifndef CHAT_MESSAGE_HPP
#define CHAT_MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

enum chat_commands { MESSAGE = 0, SPAWN_ITEM = 1, UPDATE_ITEM = 2, SPAWN_ACTOR =3 };

struct command {
    chat_commands com = MESSAGE;

    float x, y, z;
    float rot_x, rot_y, rot_z;
    float angle;
    int item;
    int actor_id;
    std::string msg = "non";

    void set_cords(float x_t, float y_t, float z_t) {
        x = x_t;
        y = y_t;
        z = z_t;
    }

    void set_rot(float x_t, float y_t, float z_t, float angle_t) {
        rot_x = x_t;
        rot_y = y_t;
        rot_z = z_t;
        angle = angle_t;
    }
};


class chat_message
{
public:
  enum { header_length = 4 };
  enum { max_body_length = 512 };

  chat_message()
    : body_length_(0)
  {
  }

  const char* data() const
  {
    return data_;
  }

  char* data()
  {
    return data_;
  }

  std::size_t length() const
  {
    return header_length + body_length_;
  }

  const char* body() const
  {
    return data_ + header_length;
  }

  char* body()
  {
    return data_ + header_length;
  }

  std::size_t body_length() const
  {
    return body_length_;
  }

  void body_length(std::size_t new_length)
  {
    body_length_ = new_length;
    if (body_length_ > max_body_length)
      body_length_ = max_body_length;
  }

  bool decode_header()
  {
    char header[header_length + 1] = "";
    std::strncat(header, data_, header_length);
    body_length_ = std::atoi(header);
    if (body_length_ > max_body_length)
    {
      body_length_ = 0;
      return false;
    }
    return true;
  }

  void encode_header()
  {
    char header[header_length + 1] = "";
    std::sprintf(header, "%4d", static_cast<int>(body_length_));
    std::memcpy(data_, header, header_length);
  }

private:
  char data_[header_length + max_body_length];
  std::size_t body_length_;
};



#endif // CHAT_MESSAGE_HPP