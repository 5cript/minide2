class LocalPersistence
{
    constructor(home, fs)
    {
        this.persistence = {
            version: 1,
            hosts: [
            ]
        }
        this.home = home;
        this.fs = fs
    }

    setLastWorkspace(currentHost, lastWs)
    {
        const index = this.persistence.hosts.findIndex(elem => {
            return elem.host === currentHost.host && elem.port === currentHost.port
        });
        if (index >= 0)
        {
            this.persistence.hosts[index].lastWorkspace = lastWs;
        }
        else
        {
            this.persistence.hosts.push({
                ...currentHost,
                lastWorkspace: lastWs
            })
        }

        this.save();
    }
    // on last active workspace
    setLastActive(currentHost, activeProject)
    {
        const index = this.persistence.hosts.findIndex(elem => {
            return elem.host === currentHost.host && elem.port === currentHost.port
        });
        if (index >= 0)
        {
            this.persistence.hosts[index].lastActiveOfLastWorkspace = activeProject;
        }
        else
        {
            this.persistence.hosts.push({
                ...currentHost,
                lastActiveOfLastWorkspace: activeProject
            })
        }
    }
    getLastActive(currentHost)
    {
        const index = this.persistence.hosts.findIndex(elem => {
            return elem.host === currentHost.host && elem.port === currentHost.port
        });
        if (index >= 0)
            return this.persistence.hosts[index].lastActiveOfLastWorkspace;
        return null;
    }
    getLastWorspace(currentHost)
    {
        const index = this.persistence.hosts.findIndex(elem => {
            return elem.host === currentHost.host && elem.port === currentHost.port
        });
        if (index >= 0)
            return this.persistence.hosts[index].lastWorkspace;
        return null;
    }
    value()
    {
        return this.persistence;
    }
    load() 
    {
        const path = this.home + "/local_persistence.json";
        if (this.fs.existsSync(path))
            this.persistence = JSON.parse(this.fs.readFileSync(path, 'utf8'));
    }
    save()
    {
        console.log(this.home + "/local_persistence.json")
        this.fs.writeFileSync(this.home + "/local_persistence.json", JSON.stringify(this.persistence, null, 4));
    }
}

export default LocalPersistence