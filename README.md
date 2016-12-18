# lob

Your sshd totp implementation: secure your living-roomfrom peeps who see you do the same thing again and again.

<pre>
command="~/foolslob/lob-login /bin/bash" ssh-rsa AAAA...blahblahblah
</pre>

Use gensecret to reap some entropy.

<pre>
PUZZLE=123456 ssh user@no-questing-fingers.local
</pre>

Or

<pre>
PUZZLE=$(/bin/puzzle) ssh user@the-media-center-is-really-my-personal-github-and-I-have-sudo-rights
</pre>

If you're worried, just delete puzzle locally and keep it on your phone.

This is an absurd means to make secure the obscure, and obscure the plainly seen patterns of precocious youth.

My only saving grace maybe that this implementation is a darn sight easier to read than that in Google Authenticator!
