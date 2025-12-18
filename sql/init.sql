DROP TABLE IF EXISTS daily_metrics;
DROP TABLE IF EXISTS activity_sessions;
DROP TABLE IF EXISTS editors;
DROP TABLE IF EXISTS languages;
DROP TABLE IF EXISTS projects;
DROP TABLE IF EXISTS developers;

-- 1. Developers Table
CREATE TABLE developers (
    developer_id SERIAL PRIMARY KEY,
    nickname VARCHAR(100) NOT NULL UNIQUE,
    email VARCHAR(150) UNIQUE,
    country VARCHAR(50)
);

-- 2. Projects Table
CREATE TABLE projects (
    project_id SERIAL PRIMARY KEY,
    name VARCHAR(150) NOT NULL UNIQUE,
    repo_url TEXT,
    is_active BOOLEAN DEFAULT TRUE
);

-- 3. Languages Table
CREATE TABLE languages (
    language_id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL UNIQUE
);

-- 4. Editors Table
CREATE TABLE editors (
    editor_id SERIAL PRIMARY KEY,
    name VARCHAR(120) NOT NULL,
    version VARCHAR(50),
    UNIQUE (name, version)
);

-- 5. Activity Sessions Table
CREATE TABLE activity_sessions (
    activity_id SERIAL PRIMARY KEY,
    developer_id INT REFERENCES developers(developer_id) ON DELETE CASCADE,
    project_id INT REFERENCES projects(project_id) ON DELETE SET NULL,
    language_id INT REFERENCES languages(language_id) ON DELETE SET NULL,
    editor_id INT REFERENCES editors(editor_id) ON DELETE SET NULL,
    start_time TIMESTAMP NOT NULL,
    duration_minutes INT NOT NULL CHECK (duration_minutes > 0),
    branch VARCHAR(120),
    os VARCHAR(60),
    is_remote BOOLEAN DEFAULT FALSE,
    notes TEXT
);

-- 6. Daily Metrics Table
CREATE TABLE daily_metrics (
    metric_id SERIAL PRIMARY KEY,
    developer_id INT REFERENCES developers(developer_id) ON DELETE CASCADE,
    day DATE NOT NULL,
    coding_minutes INT NOT NULL DEFAULT 0 CHECK (coding_minutes >= 0),
    keystrokes INT NOT NULL DEFAULT 0 CHECK (keystrokes >= 0),
    insertions INT NOT NULL DEFAULT 0 CHECK (insertions >= 0),
    deletions INT NOT NULL DEFAULT 0 CHECK (deletions >= 0),
    UNIQUE (developer_id, day)
);

-- Seed Data

INSERT INTO developers (nickname, email, country) VALUES
('alice', 'alice@example.com', 'USA'),
('bob', 'bob@example.com', 'Germany'),
('carol', 'carol@example.com', 'Canada'),
('dave', 'dave@example.com', 'Poland'),
('eve', 'eve@example.com', 'UK');

INSERT INTO projects (name, repo_url, is_active) VALUES
('ChronoTrack', 'https://github.com/example/chronotrack', TRUE),
('DeepSearch', 'https://github.com/example/deepsearch', TRUE),
('PixelWave', 'https://github.com/example/pixelwave', TRUE),
('InfraTools', 'https://github.com/example/infratools', TRUE),
('LegacyMonolith', 'https://github.com/example/legacy', FALSE);

INSERT INTO languages (name) VALUES
('Python'),
('Go'),
('C++'),
('TypeScript'),
('Rust'),
('SQL');

INSERT INTO editors (name, version) VALUES
('VS Code', '1.84'),
('CLion', '2023.2'),
('Neovim', '0.9'),
('IntelliJ IDEA', '2023.2'),
('Vim', '9.0');

-- Helper inline selects for readability
-- Activity Sessions
INSERT INTO activity_sessions (developer_id, project_id, language_id, editor_id, start_time, duration_minutes, branch, os, is_remote) VALUES
((SELECT developer_id FROM developers WHERE nickname = 'alice'),
 (SELECT project_id FROM projects WHERE name = 'ChronoTrack'),
 (SELECT language_id FROM languages WHERE name = 'Python'),
 (SELECT editor_id FROM editors WHERE name = 'VS Code'),
 '2025-01-05 09:00:00', 120, 'main', 'macOS', FALSE),
((SELECT developer_id FROM developers WHERE nickname = 'alice'),
 (SELECT project_id FROM projects WHERE name = 'DeepSearch'),
 (SELECT language_id FROM languages WHERE name = 'Go'),
 (SELECT editor_id FROM editors WHERE name = 'VS Code'),
 '2025-01-06 10:00:00', 75, 'feature/auth', 'macOS', TRUE),
((SELECT developer_id FROM developers WHERE nickname = 'bob'),
 (SELECT project_id FROM projects WHERE name = 'InfraTools'),
 (SELECT language_id FROM languages WHERE name = 'Rust'),
 (SELECT editor_id FROM editors WHERE name = 'CLion'),
 '2025-01-05 11:30:00', 90, 'refactor/cli', 'Linux', FALSE),
((SELECT developer_id FROM developers WHERE nickname = 'carol'),
 (SELECT project_id FROM projects WHERE name = 'PixelWave'),
 (SELECT language_id FROM languages WHERE name = 'TypeScript'),
 (SELECT editor_id FROM editors WHERE name = 'VS Code'),
 '2025-01-06 14:00:00', 60, 'ui/rebrand', 'Windows', TRUE),
((SELECT developer_id FROM developers WHERE nickname = 'dave'),
 (SELECT project_id FROM projects WHERE name = 'LegacyMonolith'),
 (SELECT language_id FROM languages WHERE name = 'C++'),
 (SELECT editor_id FROM editors WHERE name = 'CLion'),
 '2025-01-02 13:15:00', 150, 'bugfix/memory', 'Linux', FALSE),
((SELECT developer_id FROM developers WHERE nickname = 'eve'),
 (SELECT project_id FROM projects WHERE name = 'ChronoTrack'),
 (SELECT language_id FROM languages WHERE name = 'Python'),
 (SELECT editor_id FROM editors WHERE name = 'Vim'),
 '2025-01-03 08:45:00', 45, 'main', 'macOS', TRUE),
((SELECT developer_id FROM developers WHERE nickname = 'bob'),
 (SELECT project_id FROM projects WHERE name = 'DeepSearch'),
 (SELECT language_id FROM languages WHERE name = 'SQL'),
 (SELECT editor_id FROM editors WHERE name = 'IntelliJ IDEA'),
 '2025-01-04 16:20:00', 50, 'analytics', 'Linux', FALSE),
((SELECT developer_id FROM developers WHERE nickname = 'carol'),
 (SELECT project_id FROM projects WHERE name = 'PixelWave'),
 (SELECT language_id FROM languages WHERE name = 'TypeScript'),
 (SELECT editor_id FROM editors WHERE name = 'VS Code'),
 '2025-01-08 09:30:00', 110, 'ui/perf', 'Windows', TRUE),
((SELECT developer_id FROM developers WHERE nickname = 'alice'),
 (SELECT project_id FROM projects WHERE name = 'InfraTools'),
 (SELECT language_id FROM languages WHERE name = 'Rust'),
 (SELECT editor_id FROM editors WHERE name = 'Neovim'),
 '2025-01-07 07:40:00', 80, 'ops/metrics', 'macOS', FALSE),
((SELECT developer_id FROM developers WHERE nickname = 'eve'),
 (SELECT project_id FROM projects WHERE name = 'ChronoTrack'),
 (SELECT language_id FROM languages WHERE name = 'Go'),
 (SELECT editor_id FROM editors WHERE name = 'VS Code'),
 '2025-01-09 10:15:00', 95, 'feat/reporting', 'macOS', TRUE),
((SELECT developer_id FROM developers WHERE nickname = 'bob'),
 (SELECT project_id FROM projects WHERE name = 'InfraTools'),
 (SELECT language_id FROM languages WHERE name = 'Rust'),
 (SELECT editor_id FROM editors WHERE name = 'CLion'),
 '2025-01-09 12:00:00', 60, 'refactor/cli', 'Linux', FALSE),
((SELECT developer_id FROM developers WHERE nickname = 'alice'),
 (SELECT project_id FROM projects WHERE name = 'DeepSearch'),
 (SELECT language_id FROM languages WHERE name = 'SQL'),
 (SELECT editor_id FROM editors WHERE name = 'VS Code'),
 '2025-01-10 15:00:00', 40, 'analytics', 'macOS', TRUE);

INSERT INTO daily_metrics (developer_id, day, coding_minutes, keystrokes, insertions, deletions) VALUES
((SELECT developer_id FROM developers WHERE nickname = 'alice'), '2025-01-05', 195, 12000, 8000, 3000),
((SELECT developer_id FROM developers WHERE nickname = 'alice'), '2025-01-06', 130, 9000, 6000, 2000),
((SELECT developer_id FROM developers WHERE nickname = 'bob'), '2025-01-05', 140, 9500, 6200, 2100),
((SELECT developer_id FROM developers WHERE nickname = 'bob'), '2025-01-09', 120, 8700, 5400, 1900),
((SELECT developer_id FROM developers WHERE nickname = 'carol'), '2025-01-06', 100, 7600, 5200, 1800),
((SELECT developer_id FROM developers WHERE nickname = 'carol'), '2025-01-08', 160, 11000, 7200, 2500),
((SELECT developer_id FROM developers WHERE nickname = 'dave'), '2025-01-02', 150, 8000, 5500, 1900),
((SELECT developer_id FROM developers WHERE nickname = 'eve'), '2025-01-03', 70, 4200, 2600, 1000),
((SELECT developer_id FROM developers WHERE nickname = 'eve'), '2025-01-09', 95, 6400, 4000, 1600);
