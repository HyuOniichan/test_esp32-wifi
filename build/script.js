// Goal: Minify Web UI source code

const glob = require("glob");
const fs = require("fs-extra");
const path = require("path");
const htmlMinifier = require("html-minifier-terser");
const { execSync } = require("child_process");

const SRC = path.join(__dirname, "./data");
const OUT = path.join(__dirname, "../data");

function handleStaticFiles() {
    // Assume: Only 1 app.js file and 1 style.css file
    // minify JS using esbuild
    try {
        execSync(`npx esbuild ${SRC}/app.js --minify --outfile=${OUT}/app.js`);
        console.log("JS minified");
    } catch (e) {
        console.error("JS error", e);
    }

    // minify CSS
    try {
        execSync(`npx cleancss -o ${OUT}/style.css ${SRC}/style.css`);
        console.log("CSS minified");
    } catch (e) {
        console.error("CSS error", e);
    }
}

async function handleHtmlFiles() {
    // Find all html
    const files = glob.sync(`${SRC}/**/*.html`);

    for (const file of files) {
        const relPath = path.relative(SRC, file);
        const outPath = path.join(OUT, relPath);

        const html = fs.readFileSync(file, "utf8");

        const result = await htmlMinifier.minify(html, {
            collapseWhitespace: true,
            removeComments: true,
            minifyCSS: true,
            minifyJS: true
        });

        fs.ensureDirSync(path.dirname(outPath));
        fs.writeFileSync(outPath, result);

        console.log("Minified:", relPath);
    }
}

async function build() {
    console.log("Building web UI...");

    // clean output
    fs.emptyDirSync(OUT);

    // copy files
    fs.copySync(SRC, OUT);
    
    // minify css + js
    handleStaticFiles();

    // minify HTML
    try {
        await handleHtmlFiles();
        console.log("HTML minified");
    } catch (e) {
        console.error("HTML error", e);
    }

    console.log("Build done");
}

build();