#include <cstdint>
#include <cstring>
#include <functional>
#include <map>

#include "fatfs.h"

#include "file.hpp"
#include "32blit.h"
#include "executable.hpp"

void *open_file(const std::string &file, int mode) {
  FIL *f = new FIL();

  BYTE ff_mode = 0;

  if(mode & blit::OpenMode::read)
    ff_mode |= FA_READ;

  if(mode & blit::OpenMode::write)
    ff_mode |= FA_WRITE;

  if(mode == blit::OpenMode::write)
    ff_mode |= FA_CREATE_ALWAYS;

  FRESULT r = f_open(f, file.c_str(), ff_mode);

  if(r == FR_OK)
    return f;
  
  delete f;
  return nullptr;
}

int32_t read_file(void *fh, uint32_t offset, uint32_t length, char *buffer) {  
  FRESULT r = FR_OK;
  FIL *f = (FIL *)fh;

  if(offset != f_tell(f))
    r = f_lseek(f, offset);

  if(r == FR_OK){ 
    unsigned int bytes_read;
    r = f_read(f, buffer, length, &bytes_read);
    if(r == FR_OK){ 
      return bytes_read;
    }
  }
  
  return -1;
}

int32_t write_file(void *fh, uint32_t offset, uint32_t length, const char *buffer) {  
  FRESULT r = FR_OK;
  FIL *f = (FIL *)fh;

  if(offset != f_tell(f))
    r = f_lseek(f, offset);

  if(r == FR_OK) {
    unsigned int bytes_written;
    r = f_write(f, buffer, length, &bytes_written);
    if(r == FR_OK) {
      return bytes_written;
    }
  }

  return -1;
}

int32_t close_file(void *fh) {
  FRESULT r;

  r = f_close((FIL *)fh);

  delete (FIL *)fh;
  return r == FR_OK ? 0 : -1;
}

uint32_t get_file_length(void *fh)
{
  return f_size((FIL *)fh);
}

void list_files(const std::string &path, std::function<void(blit::FileInfo &)> callback) {
  auto dir = new DIR();

  if(f_opendir(dir, path.c_str()) != FR_OK)
    return;

  FILINFO ent;

  while(f_readdir(dir, &ent) == FR_OK && ent.fname[0]) {
    blit::FileInfo info;

    info.name = ent.fname;
    info.flags = 0;
    info.size = ent.fsize;

    if(ent.fattrib & AM_DIR)
      info.flags |= blit::FileFlags::directory;

    callback(info);
  }

  f_closedir(dir);
}

bool file_exists(const std::string &path) {
  FILINFO info;
  return f_stat(path.c_str(), &info) == FR_OK && !(info.fattrib & AM_DIR);
}

bool directory_exists(const std::string &path) {
  FILINFO info;
  return f_stat(path.c_str(), &info) == FR_OK && (info.fattrib & AM_DIR);
}

bool create_directory(const std::string &path) {
  FRESULT r;

  // strip trailing slash
  if(path.back() == '/')
     r = f_mkdir(path.substr(0, path.length() - 1).c_str());
  else
    r = f_mkdir(path.c_str());

  return r == FR_OK || r == FR_EXIST;
}

bool rename_file(const std::string &old_name, const std::string &new_name) {
  return f_rename(old_name.c_str(), new_name.c_str()) == FR_OK;
}

bool remove_file(const std::string &path) {
  return f_unlink(path.c_str()) == FR_OK;
}

static char save_path[32]; // max game title length is 24 + ".blit/" + "/"

const char *get_save_path() {
  std::string app_name;

  if(!directory_exists(".blit"))
    create_directory(".blit");

  if(!blit_user_code_running())
    app_name = "_firmware";
  else {
    // TODO: this probably can be used for other things
    auto game_ptr = reinterpret_cast<uint8_t *>(0x90000000 + persist.last_game_offset);

    auto header = reinterpret_cast<BlitGameHeader *>(game_ptr);

    if(header->magic == blit_game_magic) {
      auto end_ptr = game_ptr + (header->end - 0x90000000);
      if(memcmp(end_ptr, "BLITMETA", 8) == 0) {
        auto meta = reinterpret_cast<RawMetadata *>(end_ptr + 10);
        app_name = meta->title;
      }
    }

    if(app_name.empty()) {
      // fallback to offset
      app_name = std::to_string(persist.last_game_offset);
    }
  }

  snprintf(save_path, sizeof(save_path), ".blit/%s/", app_name.c_str());

  // make sure it exists
  if(!directory_exists(save_path))
    create_directory(save_path);


  return save_path;
}