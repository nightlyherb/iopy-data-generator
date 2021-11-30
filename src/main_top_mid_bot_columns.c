#include <stdint.h>
#include <stdio.h>

#include "../extern/sqlite/sqlite3.h"
#include "settings.h"

int main(void) {
  int status;
  sqlite3 *db = NULL;

  // Get the db handle.
  status = sqlite3_open(IOPY_DB_FILENAME, &db);
  if (status != SQLITE_OK) {
    fprintf(stderr, sqlite3_errmsg(db));
    return 1;
  }

  // Statements
  sqlite3_stmt *insert_stmt = NULL;

#define LOG_STATUS_AND_GOTO_ERROR()                                    \
  do {                                                                 \
    fprintf(stderr, "Line: %d, Error code: %d, Message: %s", __LINE__, \
            sqlite3_errcode(db), sqlite3_errmsg(db));                  \
    goto error;                                                        \
  } while (0)

  // Prepare and execute create statement
  const char *z_sql_create =
      "CREATE TABLE transition (\n"
      "  id INTEGER NOT NULL,\n"
      "  top INTEGER NOT NULL,\n"
      "  mid INTEGER NOT NULL,\n"
      "  next INTEGER NOT NULL,\n"
      "  bot INTEGER NOT NULL,\n"
      "  PRIMARY KEY (id)\n"
      ");\n";
  status = sqlite3_exec(db, z_sql_create, NULL, NULL, NULL);
  if (status != SQLITE_OK) {
    LOG_STATUS_AND_GOTO_ERROR();
  }

  // Prepare and execute page size
  status =
      sqlite3_exec(db, "PRAGMA page_size = 65536; VACUUM;", NULL, NULL, NULL);
  if (status != SQLITE_OK) {
    LOG_STATUS_AND_GOTO_ERROR();
  }

  // Prepare insert statement
  const char *z_sql_insert =
      "INSERT INTO transition (top, mid, next, bot)\n"
      "VALUES (?1, ?2, ?3, ?4)\n";
  status = sqlite3_prepare_v2(db, z_sql_insert, -1, &insert_stmt, NULL);
  if (status != SQLITE_OK) {
    LOG_STATUS_AND_GOTO_ERROR();
  }

  // Send rows
  for (uint64_t top = 0; top < IOPY_ROW_N; ++top) {
    fprintf(stderr, "%d/%d\n", (unsigned int)top, (unsigned int)IOPY_ROW_N);
    // Begin transaction
    status = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    if (status != SQLITE_OK) {
      LOG_STATUS_AND_GOTO_ERROR();
    }
    for (uint64_t mid = 0; mid < IOPY_ROW_N; ++mid) {
      for (uint64_t bot = 0; bot < IOPY_ROW_N; ++bot) {
        uint64_t next_raw = get_transition(top, mid, bot, IOPY_ROW_WIDTH);
        if (!is_valid_transition(top, mid, bot, next_raw, IOPY_ROW_WIDTH)) {
          continue;
        }
        uint64_t next = postprocess_transition(next_raw, IOPY_ROW_WIDTH);
        sqlite3_bind_int64(insert_stmt, 1, top);
        sqlite3_bind_int64(insert_stmt, 2, mid);
        sqlite3_bind_int64(insert_stmt, 3, next);
        sqlite3_bind_int64(insert_stmt, 4, bot);
        status = sqlite3_step(insert_stmt);
        sqlite3_clear_bindings(insert_stmt);
        sqlite3_reset(insert_stmt);
        if (status != SQLITE_DONE) {
          LOG_STATUS_AND_GOTO_ERROR();
        }
      }
    }
    // Commit transaction
    status = sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    if (status != SQLITE_OK) {
      LOG_STATUS_AND_GOTO_ERROR();
    }
  }
  fprintf(stderr, "Insert finished.\n");
  goto success;

success:
  sqlite3_finalize(insert_stmt);
  sqlite3_close(db);
  return 0;

error:
  sqlite3_finalize(insert_stmt);
  sqlite3_close(db);
  return 1;
}