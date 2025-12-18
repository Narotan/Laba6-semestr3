#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <libpq-fe.h>
#include <string>
#include <vector>

class DBManager {
public:
  DBManager();
  ~DBManager();

  bool connect(const std::string &connStr);
  void disconnect();
  bool isConnected() const;

  void listProjects();
  void listRecentActivities();
  void findSessionsByLanguage(const std::string &language, int minMinutes);
  void totalTimePerProject();
  void avgSessionLengthByDeveloper(const std::string &nickname);
  void topLanguages(int limit);
  void editorUsageShare();
  void busyDays(int minMinutes);
  void projectLeaderboardForDev(const std::string &nickname);
  void createNewActivity(const std::string &projectName,
                         const std::string &developerNick,
                         const std::string &languageName,
                         const std::string &editorName,
                         int durationMinutes,
                         const std::string &branch,
                         const std::string &os);

private:
  PGconn *conn;
  void printQueryResult(PGresult *res);
  void checkError();
};

#endif
