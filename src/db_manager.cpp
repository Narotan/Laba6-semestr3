#include "db_manager.h"
#include <iomanip>
#include <iostream>

DBManager::DBManager() : conn(nullptr) {}

DBManager::~DBManager() { disconnect(); }

bool DBManager::connect(const std::string &connStr) {
  conn = PQconnectdb(connStr.c_str());
  if (PQstatus(conn) != CONNECTION_OK) {
    std::cerr << "Connection to database failed: " << PQerrorMessage(conn)
              << std::endl;
    disconnect();
    return false;
  }
  std::cout << "Connected to database successfully.\n";
  return true;
}

void DBManager::disconnect() {
  if (conn) {
    PQfinish(conn);
    conn = nullptr;
  }
}

bool DBManager::isConnected() const {
  return conn != nullptr && PQstatus(conn) == CONNECTION_OK;
}

void DBManager::printQueryResult(PGresult *res) {
  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    std::cerr << "Query failed: " << PQerrorMessage(conn) << std::endl;
    PQclear(res);
    return;
  }

  int nFields = PQnfields(res);
  int nRows = PQntuples(res);

  for (int i = 0; i < nFields; i++) {
    std::cout << std::left << std::setw(20) << PQfname(res, i);
  }
  std::cout << "\n------------------------------------------------------------\n";

  for (int i = 0; i < nRows; i++) {
    for (int j = 0; j < nFields; j++) {
      std::cout << std::left << std::setw(20) << PQgetvalue(res, i, j);
    }
    std::cout << "\n";
  }
  std::cout << "\n(" << nRows << " строк)\n";

  PQclear(res);
}

void DBManager::listProjects() {
  std::cout << "\nПроекты и общее время кодирования:\n";
  const char *sql = 
      "SELECT p.name, "
      "       SUM(a.duration_minutes) AS total_minutes "
      "FROM projects p "
      "LEFT JOIN activity_sessions a ON p.project_id = a.project_id "
      "GROUP BY p.name "
      "ORDER BY total_minutes DESC NULLS LAST";
  PGresult *res = PQexec(conn, sql);
  printQueryResult(res);
}

void DBManager::listRecentActivities() {
  std::cout << "\nПоследние 10 сессий разработки:\n";
  const char *sql =
      "SELECT d.nickname, p.name AS project, l.name AS language, "
      "       e.name AS editor, a.duration_minutes, a.start_time "
      "FROM activity_sessions a "
      "JOIN developers d ON a.developer_id = d.developer_id "
      "JOIN projects p ON a.project_id = p.project_id "
      "JOIN languages l ON a.language_id = l.language_id "
      "JOIN editors e ON a.editor_id = e.editor_id "
      "ORDER BY a.start_time DESC "
      "LIMIT 10";
  PGresult *res = PQexec(conn, sql);
  printQueryResult(res);
}

void DBManager::findSessionsByLanguage(const std::string &language, int minMinutes) {
  std::cout << "\nСессии на " << language << " >= " << minMinutes << " минут:\n";
  std::string sql =
      "SELECT d.nickname, p.name AS project, a.duration_minutes, a.start_time "
      "FROM activity_sessions a "
      "JOIN developers d ON a.developer_id = d.developer_id "
      "JOIN projects p ON a.project_id = p.project_id "
      "JOIN languages l ON a.language_id = l.language_id "
      "WHERE l.name = '" + language + "' "
      "  AND a.duration_minutes >= " + std::to_string(minMinutes) + " "
      "ORDER BY a.start_time DESC";
  PGresult *res = PQexec(conn, sql.c_str());
  printQueryResult(res);
}

void DBManager::totalTimePerProject() {
  std::cout << "\nОбщее время кодирования по проектам:\n";
  const char *sql =
      "SELECT p.name, "
      "       SUM(a.duration_minutes) AS total_minutes, "
      "       COUNT(a.activity_id) AS session_count "
      "FROM projects p "
      "LEFT JOIN activity_sessions a ON p.project_id = a.project_id "
      "GROUP BY p.name "
      "ORDER BY total_minutes DESC NULLS LAST";
  PGresult *res = PQexec(conn, sql);
  printQueryResult(res);
}

void DBManager::avgSessionLengthByDeveloper(const std::string &nickname) {
  std::cout << "\nСредняя длительность сессии разработчика " << nickname << ":\n";
  std::string sql =
      "SELECT d.nickname, AVG(a.duration_minutes) AS avg_minutes "
      "FROM developers d "
      "JOIN activity_sessions a ON d.developer_id = a.developer_id "
      "WHERE d.nickname = '" + nickname + "' "
      "GROUP BY d.nickname";
  PGresult *res = PQexec(conn, sql.c_str());
  printQueryResult(res);
}

void DBManager::topLanguages(int limit) {
  std::cout << "\nТоп " << limit << " языков по времени кодирования:\n";
  std::string sql =
      "SELECT l.name, SUM(a.duration_minutes) AS total_minutes "
      "FROM languages l "
      "JOIN activity_sessions a ON l.language_id = a.language_id "
      "GROUP BY l.name "
      "ORDER BY total_minutes DESC "
      "LIMIT " + std::to_string(limit);
  PGresult *res = PQexec(conn, sql.c_str());
  printQueryResult(res);
}

void DBManager::editorUsageShare() {
  std::cout << "\nИспользование редакторов (процент времени):\n";
  const char *sql =
      "SELECT e.name, "
      "       SUM(a.duration_minutes) AS total_minutes, "
      "       ROUND(100.0 * SUM(a.duration_minutes) / "
      "             (SELECT SUM(duration_minutes) FROM activity_sessions), 2) AS percent "
      "FROM editors e "
      "JOIN activity_sessions a ON e.editor_id = a.editor_id "
      "GROUP BY e.name "
      "HAVING SUM(a.duration_minutes) > 0 "
      "ORDER BY total_minutes DESC";
  PGresult *res = PQexec(conn, sql);
  printQueryResult(res);
}

void DBManager::busyDays(int minMinutes) {
  std::cout << "\nДни с кодированием >= " << minMinutes << " минут:\n";
  std::string sql =
      "SELECT dm.day, d.nickname, dm.coding_minutes "
      "FROM daily_metrics dm "
      "JOIN developers d ON dm.developer_id = d.developer_id "
      "WHERE dm.coding_minutes >= " + std::to_string(minMinutes) + " "
      "ORDER BY dm.coding_minutes DESC";
  PGresult *res = PQexec(conn, sql.c_str());
  printQueryResult(res);
}

void DBManager::projectLeaderboardForDev(const std::string &nickname) {
  std::cout << "\nЛидерборд проектов для разработчика " << nickname << ":\n";
  std::string sql =
      "SELECT p.name, SUM(a.duration_minutes) AS total_minutes "
      "FROM projects p "
      "JOIN activity_sessions a ON p.project_id = a.project_id "
      "WHERE a.developer_id = ("
      "  SELECT developer_id FROM developers WHERE nickname = '" + nickname + "'"
      ") "
      "GROUP BY p.name "
      "ORDER BY total_minutes DESC";
  PGresult *res = PQexec(conn, sql.c_str());
  printQueryResult(res);
}

void DBManager::createNewActivity(const std::string &projectName,
                                  const std::string &developerNick,
                                  const std::string &languageName,
                                  const std::string &editorName,
                                  int durationMinutes,
                                  const std::string &branch,
                                  const std::string &os) {
  std::cout << "\nДобавление новой сессии...\n";

  std::string sql =
      "INSERT INTO activity_sessions "
      "(developer_id, project_id, language_id, editor_id, "
      " start_time, duration_minutes, branch, os) "
      "VALUES ("
      "(SELECT developer_id FROM developers WHERE nickname = '" + developerNick + "'), "
      "(SELECT project_id FROM projects WHERE name = '" + projectName + "'), "
      "(SELECT language_id FROM languages WHERE name = '" + languageName + "'), "
      "(SELECT editor_id FROM editors WHERE name = '" + editorName + "' LIMIT 1), "
      "NOW(), " + std::to_string(durationMinutes) + ", "
      "'" + branch + "', '" + os + "')";

  PGresult *res = PQexec(conn, sql.c_str());

  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    std::cerr << "Ошибка добавления: " << PQerrorMessage(conn) << std::endl;
  } else {
    std::cout << "Сессия успешно добавлена!\n";
  }
  PQclear(res);
}
