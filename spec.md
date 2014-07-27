gtorrent-core API
----
### Torrent
- `Torrent(string path)`

Constructs a torrent object with internal bindings to libtorrent. `path` is a magnet link, path to .torrent file on local filesystem, or URL to .torrent file online. Will throw an exception in case of error.

- `bool pollEvent(gt::Event &event)`

TODO: add description 

#### Getters

- `string& getPath()`

Gets the path passed in to the constructor.

- `boost::int64_t getActiveTime()`

Returns the number of seconds the torrent has been active.

- `boost::int64_t getEta()` 

Returns the ETA in seconds for the torrent.

- `vector<bool> getPieces()` 

Returns a vector of bools representing the pieces - true if we have it and false otherwise.

- `float getTotalProgress()`

Returns percentage of all files downloading in this torrent.

- `unsigned int getUploadRate()`

Returns the current upload rate of the torrent.

- `unsigned int getDownloadRate()`

Returns the current download rate of the torrent

- `unsigned int getPPMProgress()`

Returns the progress in PPM of all files downloading in this torrent.

- `unsigned int getTotalSeeders()`

Returns the current number of seeders attached to the torrent.

- `unsigned int getTotalPeers()`

Returns the current number of peers attached to the torrent.

- `unsigned int getTotalLeechers()`

Returns the current number of leechers attached to the torrent.

- `boost::int64_t getTotalUploaded()`

Returns the current amount of data uploaded for this torrent.

- `boost::int64_t getTotalDownloaded()`

Returns the current amount of data downloaded for this torrent.

- `boost::int64_t getSize()`

Returns the total size of files in this torrent

- `boost::int64_t getTimeRemaining()`

Returns the time remaining in seconds

- `float getTotalRatio()`

Returns the ratio (uploaded/downloaded) for this torrent

- `libtorrent::torrent_status::state_t getState()`

Returns the current torrent state (downloading, queueing, seeding, etc) as a libtorrent `state_t`.

- `string getTextState()`

Returns a the torrent state as a string.

- `string getCurrentTrackerURL()`

Returns the URL of the last working tracker

- `void torrentForceRecheck()`

Force a recheck of the torrent

- `boost::int64_t getRemaining()`

Returns the total size of files remaining to download in the torrent.

- `bool isPaused()`

#### Setters

- `void resume()`

Resumes the torrent.

- `void pause()`

Pauses the torrent.

- `void setSavePath(string savepath)`

Sets the download location for this torrent.

